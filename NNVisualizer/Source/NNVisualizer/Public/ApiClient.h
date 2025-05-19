

#pragma once

#include "CoreMinimal.h"
#include "HiddenLayerNeuron.h"
#include "Neuron_Layer.h"
#include "HttpModule.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"


typedef struct LayerStructure {
	int32 numberOfNeurons;
	int32 height;
	int32 width;
	FVector position;
};
typedef struct NetworkStructure {
	int32 numberOfLayers;
	TArray<LayerStructure>	layers;
};
DECLARE_DELEGATE(FOnStructureInitFinished);
DECLARE_DELEGATE(FOnStructureUpdateFinished);


class NNVISUALIZER_API UApiClient
{
	// Sets default values for this component's properties
	NetworkStructure m_networkStructure;
	TArray<LayerData>* m_layersData;
	TArray<TArray<TArray<HiddenLayerNeuronData*>>>* m_hiddenLayersWeights;
	int32 m_frameCnt = 0;
	FEvent* responseEvent;
	void sendStructInitRequest();
	void sendStructWeightsRequest();
	void sendStructValuesRequest();
	void sendStructBiasRequest();

	void StructInitHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful);

	void StructWeightsHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful);
	void StructValuesHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful);
	void StructBiasHttpResponse(FHttpRequestPtr _request, FHttpResponsePtr _response, bool _wasSuccessful);
public:
	FOnStructureInitFinished OnStructureInitFinished;
	FOnStructureUpdateFinished OnStructureUpdateFinished;
	void initStucture(
		TArray<LayerData>* _layersData,
		TArray< TArray<TArray<HiddenLayerNeuronData*>>>* _hiddenLayersWeights
	);

	void update();

	UApiClient();
	~UApiClient();
};
