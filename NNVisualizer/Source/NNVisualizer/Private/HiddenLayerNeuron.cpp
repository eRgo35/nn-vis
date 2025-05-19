


#include "HiddenLayerNeuron.h"


ALineActor* AHiddenLayerNeuron::spawnLineActor(FVector Start, FVector End)
{
    FVector midpoint = (Start + End) / 2.0f;

    FVector direction = (End - Start);
    float length = direction.Size();
    FRotator rotation = direction.Rotation();
    auto tmp = GetWorld()->SpawnActor<ALineActor>(
        ALineActor::StaticClass(),
        midpoint,
        rotation
    );

    tmp->SetActorScale3D(FVector(length / 100.0f, 0.001f, 0.001f));
    return tmp;
}

void AHiddenLayerNeuron::init(TArray<HiddenLayerNeuronData*>& prevLayerNeuronsData)
{
    m_prevLayerNeuronsData = prevLayerNeuronsData;
    for (auto n : m_prevLayerNeuronsData) {
        n->Line = spawnLineActor(this->GetActorLocation(), n->Location);
    }
}



void AHiddenLayerNeuron::updateWeights()
{
    for (auto& n : m_prevLayerNeuronsData) {
        n->Line->SetWeight(n->weight);

    }
}
void AHiddenLayerNeuron::updateLocation()
{
    for (auto& n : m_prevLayerNeuronsData) {
        FVector midpoint = (this->GetActorLocation() + n->Location) / 2.0f;

        FVector direction = (n->Location - this->GetActorLocation());
        float length = direction.Size();
        FRotator rotation = direction.Rotation();
        n->Line->SetActorLocation(midpoint);
        n->Line->SetActorRotation(rotation);
        n->Line->SetActorScale3D(FVector(length / 100.0f, 0.001f, 0.001f));

    }
}
void AHiddenLayerNeuron::BeginPlay()
{
    Super::BeginPlay();
}

void AHiddenLayerNeuron::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}