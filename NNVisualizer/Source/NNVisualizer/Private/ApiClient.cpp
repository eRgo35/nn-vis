


#include "ApiClient.h"


// Sets default values for this component's properties
UApiClient::UApiClient()
{
	
}
UApiClient::~UApiClient()
{

}

void UApiClient::requestInitStructure() {
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
	request->SetVerb(TEXT("GET"));
	request->SetURL(TEXT("http://127.0.0.1:8000/structure"));
	request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	request->OnProcessRequestComplete().BindLambda(
		[&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) mutable
		{
			if (bWasSuccessful && Response.IsValid())
			{
				FString ResponseContent = Response->GetContentAsString();
				setupInitStructure(ResponseContent);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get response from server"));
			}
		}
	);
	request->ProcessRequest();
}



void UApiClient::setupInitStructure(const FString& ResponseContent) {
	// Parse the JSON response
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseContent);
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		// Extract the number of layers
		m_networkStructure.numberOfLayers = JsonObject->GetIntegerField("numberOfLayers");
		// Extract the layers array
		const TArray<TSharedPtr<FJsonValue>>* LayersArray;
		if (JsonObject->TryGetArrayField("layers", LayersArray))
		{
			for (const TSharedPtr<FJsonValue>& LayerValue : *LayersArray)
			{
				TSharedPtr<FJsonObject> LayerObject = LayerValue->AsObject();
				if (LayerObject.IsValid())
				{
					LayerStructure layer;
					layer.height = LayerObject->GetIntegerField("height");
					layer.width = LayerObject->GetIntegerField("width");
					layer.numberOfNeurons = layer.height * layer.width;
					layer.position = FVector(
						LayerObject->GetNumberField(TEXT("X")),
						LayerObject->GetNumberField(TEXT("Y")),
						LayerObject->GetNumberField(TEXT("Z"))
					);
					m_networkStructure.layers.Add(layer);
				}
			}
		}

		OnStructureInitFinished.ExecuteIfBound();

	}
}