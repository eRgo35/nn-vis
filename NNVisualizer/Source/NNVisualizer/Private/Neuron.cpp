


#include "Neuron.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
ANeuron::ANeuron()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANeuron::BeginPlay()
{
	Super::BeginPlay();
	this->SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
	
}

// Called every frame
void ANeuron::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


