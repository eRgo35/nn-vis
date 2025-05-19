

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Neuron.h"
#include "Neuron_Layer.generated.h"

typedef struct NeuronData {
	ANeuron* neuron;
	float bias;
	float activation;
};
typedef struct LayerData {
	TArray<NeuronData*> neurons;
	int32 height;
	int32 width;
	FVector position;
};


UCLASS()
class NNVISUALIZER_API ANeuron_Layer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANeuron_Layer();
	void init(int32 height, int32 width, float neuronSpacing, const TArray< NeuronData*>& neurons, const FVector& layerPosition);
	TArray<FVector> getNeuronLocations();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual ANeuron* spawnNeuron(int32 positionX, int32 positionZ);
	TArray< NeuronData*> m_neurons;
	FVector m_layerPosition{ 0.0f, 0.0f, 0.0f };


	float m_neuronSpacing = 15.0f;
	int32 m_width = 3;
	int32 m_height = 3;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void update();
	
	
};
