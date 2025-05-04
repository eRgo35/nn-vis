

#pragma once

#include "CoreMinimal.h"
#include "Neuron_Layer.h"
#include "Hidden_Neuron_Layer.h"


class NNVISUALIZER_API Hidden_Neuron_Layer: public ANeuron_Layer
{
	GENERATED_BODY()
	TArray<TArray<HiddenLayerNeuronData*>> Neurons;
public:
	Hidden_Neuron_Layer();
	~Hidden_Neuron_Layer();
};
