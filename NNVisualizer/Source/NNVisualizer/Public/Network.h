

#pragma once

#include "Neuron_Layer.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hidden_Neuron_Layer.h"
#include "apiClient.h"
#include "Network.generated.h"

UCLASS()
class NNVISUALIZER_API ANetwork : public AActor
{
	GENERATED_BODY()
	TArray<TWeakObjectPtr<ANeuron_Layer>> m_layers;
	TArray< TWeakObjectPtr<AHidden_Neuron_Layer>> m_hiddenLayers;
	TArray<LayerData> m_layersData;

	UApiClient m_apiClient;

	TArray< TArray<TArray<HiddenLayerNeuronData*>>> m_hiddenLayersWeights;

	float m_neuronSpacing = 30.0f;

	int32 cnt = 0;
	int32 debugCnt = 0;
	
public:	
	// Sets default values for this actor's properties
	ANetwork();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TWeakObjectPtr<ANeuron_Layer> addLayer(int32 height, int32 width, int32 layer_ID, const FVector& position);
	TWeakObjectPtr<ANeuron_Layer> addHiddenLayer(int32 height, int32 width, int32 layer_ID, const FVector& position, TWeakObjectPtr<ANeuron_Layer>& prevLayer);
	void updateLayers();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void OnNetworkStructureReady();
	
	
};
