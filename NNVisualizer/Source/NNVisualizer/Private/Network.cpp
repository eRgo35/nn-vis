


#include "Network.h"


ANetwork::ANetwork()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANetwork::BeginPlay()
{
	Super::BeginPlay();

	// Bind callback
	m_apiClient.OnStructureInitFinished.BindUObject(this, &ANetwork::OnNetworkStructureReady);

	// Start async request
	m_apiClient.initStucture(&m_layersData, &m_hiddenLayersWeights);
}

TWeakObjectPtr<ANeuron_Layer> ANetwork::addLayer(int32 height, int32 width, int32 layer_ID, const FVector& position)
{
	m_layers.Add(
		GetWorld()->SpawnActor<ANeuron_Layer>(
			ANeuron_Layer::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator
		)
	);

	m_layers.Last()->init(height, width, m_neuronSpacing, m_layersData[layer_ID].neurons, position);
	return m_layers.Last();
}

TWeakObjectPtr<ANeuron_Layer> ANetwork::addHiddenLayer(int32 height, int32 width, int32 layer_ID, const FVector& position, TWeakObjectPtr<ANeuron_Layer>& prevLayer)
{
	m_hiddenLayers.Add(
		GetWorld()->SpawnActor<AHidden_Neuron_Layer>(
			AHidden_Neuron_Layer::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator
		)
	);

	m_hiddenLayers.Last()->init(height, width, m_neuronSpacing, m_layersData[layer_ID].neurons, position, m_hiddenLayersWeights[layer_ID - 1], prevLayer);
	return m_hiddenLayers.Last();

}

// Called every frame
void ANetwork::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	cnt++;
	if (cnt == 60) {

		m_apiClient.OnStructureUpdateFinished.BindUObject(this, &ANetwork::updateLayers);

		// Start async request
		m_apiClient.update();
		cnt = 0;
	}

}
void ANetwork::updateLayers()
{
	for (auto l : m_layers) {
		l->update();
	}
}

void ANetwork::OnNetworkStructureReady()
{
	if (m_layersData.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No layers data found!"));
		return;
	}

	TArray<TWeakObjectPtr<ANeuron_Layer>> layerPointers;
	int32 lastIndex = layerPointers.Add(addLayer(m_layersData[0].height, m_layersData[0].width, 0, m_layersData[0].position));

	for (int32 i = 1; i < m_layersData.Num(); i++)
	{
		lastIndex = layerPointers.Add(addHiddenLayer(
			m_layersData[i].height,
			m_layersData[i].width,
			i,
			m_layersData[i].position,
			layerPointers[i - 1]
		));
	}

	for (auto& hl : m_hiddenLayers)
	{
		m_layers.Add(hl);
	}

	UE_LOG(LogTemp, Log, TEXT("Network structure created successfully"));
}

