


#include "Hidden_Neuron_Layer.h"


void AHidden_Neuron_Layer::init(int32 height, int32 width, float neuronSpacing, const TArray<NeuronData*>& neurons, const FVector& layerPosition, TArray<TArray<HiddenLayerNeuronData*>>& hiddenLayerData, TWeakObjectPtr<ANeuron_Layer> prevLayer)
{
	//Super::init(height, width, neuronSpacing, neurons, layerPosition);
	m_hiddenLayerData = hiddenLayerData;
	m_prevLayer = prevLayer;
	UE_LOG(LogTemp, Log, TEXT("prev layer pointer: %p"), m_prevLayer.Get());
	TArray<FVector> locations = prevLayer->getNeuronLocations();
	for (auto& layer : m_hiddenLayerData) {
		for (int32 i = 0; i < locations.Num(); i++) {
			layer[i]->Location = locations[i];
		}

	}
	for (int32 i = 0; i < m_neurons.Num(); i++) {
		auto casted = dynamic_cast<AHiddenLayerNeuron*>(m_neurons[i]->neuron);
		if (casted)
			casted->init(hiddenLayerData[i]);
	}
}
void AHidden_Neuron_Layer::update()
{
	//Super::update();
	TArray<FVector> locations = m_prevLayer->getNeuronLocations();
	for (auto& layer : m_hiddenLayerData) {
		for (int32 i = 0; i < locations.Num(); i++) {
			layer[i]->Location = locations[i];
		}

	}
	for (auto neuron : m_neurons) {
		auto casted = dynamic_cast<AHiddenLayerNeuron*>(neuron->neuron);
		if (casted) {
			casted->updateWeights();

			casted->updateLocation();
		}
	}

}
ANeuron* AHidden_Neuron_Layer::spawnNeuron(int32 positionX, int32 positionZ)
{
	return  GetWorld()->SpawnActor<AHiddenLayerNeuron>(
		AHiddenLayerNeuron::StaticClass(),
		FVector(
			positionX * m_neuronSpacing + m_layerPosition.X,
			m_neurons[positionX * m_width + positionZ]->bias + m_layerPosition.Y,
			positionZ * m_neuronSpacing + m_layerPosition.Z
		),
		FRotator::ZeroRotator
	);



}
