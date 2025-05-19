

#pragma once

#include "CoreMinimal.h"
#include "Neuron_Layer.h"
#include "LineActor.h"
#include "HiddenLayerNeuron.generated.h"

typedef struct HiddenLayerNeuronData {
	FVector Location;
	float weight;
	ALineActor* Line;
};

UCLASS()
class NNVISUALIZER_API AHiddenLayerNeuron : public ANeuron
{
	GENERATED_BODY()
	TArray<HiddenLayerNeuronData*> m_prevLayerNeuronsData;
	ALineActor* spawnLineActor(FVector Start, FVector End);

public:
	void init(TArray<HiddenLayerNeuronData*>& prevLayerNeuronsData);

	void updateWeights();

	void updateLocation();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);
};
