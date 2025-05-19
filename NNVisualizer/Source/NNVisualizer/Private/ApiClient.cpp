


#include "ApiClient.h"


void UApiClient::initStucture(TArray<LayerData>* _layersData, TArray<TArray<TArray<HiddenLayerNeuronData*>>>* _hiddenLayersWeights)
{
    m_layersData = _layersData;
    m_hiddenLayersWeights = _hiddenLayersWeights;
    responseEvent = FPlatformProcess::GetSynchEventFromPool(true);
    // Send the request
    sendStructInitRequest();


}

void UApiClient::update()
{
    sendStructWeightsRequest();
    sendStructValuesRequest();
    sendStructBiasRequest();

    m_frameCnt++;
    if (OnStructureUpdateFinished.IsBound())
    {
        OnStructureUpdateFinished.Execute();
    }
}

UApiClient::UApiClient()
{
}

UApiClient::~UApiClient()
{
}
void UApiClient::sendStructInitRequest()
{
    // Create HTTP request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    // Set URL (FastAPI Server)
    Request->SetURL(TEXT("http://127.0.0.1:8000/structure"));
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Bind the response callback
    Request->OnProcessRequestComplete().BindRaw(this, &UApiClient::StructInitHttpResponse);

    // Send the request
    Request->ProcessRequest();
}

void UApiClient::StructInitHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful)
{
    if (_wasSuccessful && _response.IsValid())
    {
        FString ResponseString = _response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);

        // Parse JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {

            m_networkStructure.numberOfLayers = JsonObject->GetIntegerField(TEXT("numberOfLayers"));  // Extract "numberOfLayers" field
            FString numberOfLayers = JsonObject->GetStringField(TEXT("numberOfLayers"));  // Extract "numberOfLayers" field
            UE_LOG(LogTemp, Log, TEXT("numberOfLayers: %s"), *numberOfLayers);


            TSharedPtr<FJsonObject> LayersObject = JsonObject->GetObjectField("layers");  // Extract "layers" field
            for (auto& layer : LayersObject->Values)
            {

                LayerStructure layerStructure;
                FString LayerIndex = layer.Key;
                TSharedPtr<FJsonObject> layerData = layer.Value->AsObject();
                TSharedPtr<FJsonObject> positionObject = layerData->GetObjectField("position");
                FVector positionTmp = FVector(
                    positionObject->GetNumberField(TEXT("x")),
                    positionObject->GetNumberField(TEXT("y")),
                    positionObject->GetNumberField(TEXT("z"))
                );

                FString type = layerData->GetStringField(TEXT("type"));
                layerStructure.height = layerData->GetIntegerField(TEXT("height"));
                layerStructure.width = layerData->GetIntegerField(TEXT("width"));
                layerStructure.numberOfNeurons = layerStructure.height * layerStructure.width;
                layerStructure.position = positionTmp;

                UE_LOG(LogTemp, Log, TEXT("Layer %s: Type=%s, Height=%d, Width=%d"), *LayerIndex, *type, layerStructure.height, layerStructure.width);

                m_networkStructure.layers.Add(layerStructure);
            }
            AsyncTask(ENamedThreads::GameThread, [this]()
                {

                    for (const auto& layer : m_networkStructure.layers)
                    {
                        LayerData tmp;
                        for (int32 i = 0; i < layer.numberOfNeurons; i++)
                            tmp.neurons.Add(new NeuronData{ nullptr, float(0), float(0) });
                        tmp.height = layer.height;
                        tmp.width = layer.width;
                        tmp.position = layer.position;

                        m_layersData->Add(tmp);
                    }

                    for (int32 i = 1; i < m_networkStructure.numberOfLayers; i++)
                    {
                        TArray<TArray<HiddenLayerNeuronData*>> tmp;
                        for (int32 j = 0; j < m_networkStructure.layers[i].numberOfNeurons; j++)
                        {
                            TArray<HiddenLayerNeuronData*> tmpHiddenLayer;
                            for (int32 k = 0; k < m_networkStructure.layers[i - 1].numberOfNeurons; k++)
                                tmpHiddenLayer.Add(new HiddenLayerNeuronData{ m_networkStructure.layers[i - 1].position, float(0), nullptr });
                            tmp.Add(tmpHiddenLayer);
                        }
                        m_hiddenLayersWeights->Add(tmp);
                    }


                    if (OnStructureInitFinished.IsBound())
                    {
                        OnStructureInitFinished.Execute();
                    }
                });
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to contact server!"));
    }
}

void UApiClient::sendStructWeightsRequest()
{
    TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    // Set URL (FastAPI Server)
    FString URL = FString::Printf(TEXT("http://127.0.0.1:8000/weights/%d"), m_frameCnt);
    Request->SetURL(URL);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Bind the response callback
    Request->OnProcessRequestComplete().BindRaw(this, &UApiClient::StructWeightsHttpResponse);

    // Send the request
    Request->ProcessRequest();

}

void UApiClient::sendStructValuesRequest()
{

    TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    // Set URL (FastAPI Server)
    FString URL = FString::Printf(TEXT("http://127.0.0.1:8000/values/%d"), m_frameCnt);

    Request->SetURL(URL);
    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Bind the response callback
    Request->OnProcessRequestComplete().BindRaw(this, &UApiClient::StructValuesHttpResponse);

    // Send the request
    Request->ProcessRequest();

}

void UApiClient::sendStructBiasRequest()
{
    TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    // Set URL (FastAPI Server)
    FString URL = FString::Printf(TEXT("http://127.0.0.1:8000/bias/%d"), m_frameCnt);

    Request->SetURL(URL);

    Request->SetVerb(TEXT("GET"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Bind the response callback
    Request->OnProcessRequestComplete().BindRaw(this, &UApiClient::StructBiasHttpResponse);

    // Send the request
    Request->ProcessRequest();
}

void UApiClient::StructWeightsHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful)
{

    if (_wasSuccessful && _response.IsValid())
    {
        FString ResponseString = _response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);

        // Parse JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {

            for (auto& Layer : JsonObject->Values)
            {
                int32 LayerIndex = FCString::Atoi(*Layer.Key);
                TSharedPtr<FJsonObject> LayerWeights = Layer.Value->AsObject();

                for (auto& neuron : LayerWeights->Values)
                {
                    int32 neuronIndex = FCString::Atoi(*neuron.Key);
                    TSharedPtr<FJsonObject> NeuronWeights = neuron.Value->AsObject();

                    for (auto& weight : NeuronWeights->Values)
                    {
                        int32 weightIndex = FCString::Atoi(*weight.Key);
                        float WeightValue = weight.Value->AsNumber();
                        // Update the weight
                        //
                        (*m_hiddenLayersWeights)[LayerIndex][neuronIndex][weightIndex]->weight = WeightValue;
                    }

                }
            }

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to contact server!"));
    }

}

void UApiClient::StructValuesHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful)
{

    if (_wasSuccessful && _response.IsValid())
    {
        FString ResponseString = _response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);

        // Parse JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {

            for (auto& Layer : JsonObject->Values)
            {
                int32 LayerIndex = FCString::Atoi(*Layer.Key);
                TSharedPtr<FJsonObject> LayerWeights = Layer.Value->AsObject();

                for (auto& neuron : LayerWeights->Values)
                {
                    int32 neuronIndex = FCString::Atoi(*neuron.Key);
                    float neuronValue = neuron.Value->AsNumber();

                    (*m_layersData)[LayerIndex].neurons[neuronIndex]->activation = neuronValue;


                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to contact server!"));
    }

}

void UApiClient::StructBiasHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful)
{

    if (_wasSuccessful && _response.IsValid())
    {
        FString ResponseString = _response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *ResponseString);

        // Parse JSON
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {

            for (auto& Layer : JsonObject->Values)
            {
                int32 LayerIndex = FCString::Atoi(*Layer.Key);
                TSharedPtr<FJsonObject> LayerWeights = Layer.Value->AsObject();

                for (auto& neuron : LayerWeights->Values)
                {
                    int32 neuronIndex = FCString::Atoi(*neuron.Key);
                    float biasValue = neuron.Value->AsNumber();

                    (*m_layersData)[LayerIndex].neurons[neuronIndex]->bias = biasValue;


                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to contact server!"));
    }
}