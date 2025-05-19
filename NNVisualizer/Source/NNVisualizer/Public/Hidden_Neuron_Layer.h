#pragma once

#include "CoreMinimal.h"
#include "Neuron_Layer.h"
#include "HiddenLayerNeuron.h"
#include "Hidden_Neuron_Layer.generated.h" // Corrected macro placement

UCLASS()
class NNVISUALIZER_API AHidden_Neuron_Layer : public ANeuron_Layer
{
GENERATED_BODY() // Corrected macro usage
TArray<TArray<HiddenLayerNeuronData*>> m_hiddenLayerData;
TWeakObjectPtr<ANeuron_Layer> m_prevLayer;

protected:
virtual ANeuron* spawnNeuron(int32 positionX, int32 positionZ) override;

public:
void init(int32 height, int32 width, float neuronSpacing, const TArray<NeuronData*>& neurons, const FVector& layerPosition, TArray<TArray<HiddenLayerNeuronData*>>& hiddenLayerData, TWeakObjectPtr<ANeuron_Layer> prevLayer);
virtual void update() override;
};
