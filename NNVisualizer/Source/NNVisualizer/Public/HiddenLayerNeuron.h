

#pragma once

#include "CoreMinimal.h"
#include "HiddenLayerNeuron.generated.h"
#include "Neuron.h"

typedef struct HiddenLayerNeuronData {
	FVector Location;
	float weight;
};

UCLASS()
class NNVISUALIZER_API HiddenLayerNeuron : public ANeuron
{
	GENERATED_BODY()
	TArray<HiddenLayerNeuronData*> m_prevLayer;
public:
	void init(TArray<HiddenLayerNeuronData*>& prevLayer);
	void updateWeights();
	void updateLocation();
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
};
