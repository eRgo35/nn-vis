


#include "Neuron_Layer.h"


// Sets default values
ANeuron_Layer::ANeuron_Layer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ANeuron_Layer::init(int32 height, int32 width, float neuronSpacing, const TArray< NeuronData*>& neurons, const FVector& layerPosition)
{
	m_height = height;
	m_width = width;
	m_neurons = neurons;
	m_neuronSpacing = neuronSpacing;
	m_layerPosition = layerPosition;

	for (int32 i = 0; i < m_height; i++) {
		for (int32 j = 0; j < m_width; j++) {
			m_neurons[i * m_width + j]->neuron = spawnNeuron(i, j);
			m_neurons[i * m_width + j]->neuron->setActivation(m_neurons[i * m_width + j]->activation);
		}
	}

}

ANeuron* ANeuron_Layer::spawnNeuron(int32 positionX, int32 positionZ)
{
	return GetWorld()->SpawnActor<ANeuron>(
		ANeuron::StaticClass(),
		FVector(
			positionX * m_neuronSpacing + m_layerPosition.X,
			m_neurons[positionX * m_width + positionZ]->bias + m_layerPosition.Y,
			positionZ * m_neuronSpacing + m_layerPosition.Z
		),
		FRotator::ZeroRotator
	);

}

TArray<FVector> ANeuron_Layer::getNeuronLocations()
{
	TArray<FVector> tmp;
	for (auto& n : m_neurons) {
		if (n->neuron) {
			tmp.Add(n->neuron->GetActorLocation());
		}
		else {
			//std::ofstream("log.txt");

			UE_LOG(LogTemp, Error, TEXT("Neuron is destroyed or invalid!"));
		}
	}

	return tmp;
}

void ANeuron_Layer::update()
{
	for (int32 i = 0; i < m_height; i++)
		for (int32 j = 0; j < m_width; j++) {
			m_neurons[i * m_width + j]->neuron->setActivation(m_neurons[i * m_width + j]->activation);
			//update bias
			m_neurons[i * m_width + j]->neuron->SetActorLocation(FVector(
				i * m_neuronSpacing + m_layerPosition.X,
				m_neurons[i * m_width + j]->bias + m_layerPosition.Y,
				j * m_neuronSpacing + m_layerPosition.Z
			));
		}
}


// Called when the game starts or when spawned
void ANeuron_Layer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANeuron_Layer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


