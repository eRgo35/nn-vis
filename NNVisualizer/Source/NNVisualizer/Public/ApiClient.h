

#pragma once

#include "CoreMinimal.h"
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
	NetworkStructure m_networkStructure;
	TArray<LayerData>* m_layersData;
	int32 m_frames = 0;
	FEvent* responseEvent;

public:	
	// Sets default values for this component's properties
	UApiClient();
	~UApiClient();
	FOnStructureInitFinished OnStructureInitFinished;
	FOnStructureUpdateFinished OnStructureUpdateFinished;
	void InitStructure(TArray<LayerData>* _layersData);
	void update();
	
};
