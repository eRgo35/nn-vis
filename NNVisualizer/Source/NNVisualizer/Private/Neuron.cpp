


#include "Neuron.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
ANeuron::ANeuron()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_cubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE_MESH"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeAsset.Succeeded()) {
		m_cubeMesh->SetStaticMesh(CubeAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial(TEXT("/Game/DynMat.DynMat"));
	if (TranslucentMaterial.Succeeded())
	{
		m_cubeMesh->SetMaterial(0, TranslucentMaterial.Object);
		m_dynamicMaterial = UMaterialInstanceDynamic::Create(m_cubeMesh->GetMaterial(0), this);
	}

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

void ANeuron::setActivation(float new_activation)
{
	this->ChangeMeshColor(FLinearColor(-new_activation, 0, new_activation));
}

void ANeuron::ChangeMeshColor(FLinearColor NewColor)
{

	m_dynamicMaterial->SetVectorParameterValue(FName("Color"), NewColor);

	m_cubeMesh->SetMaterial(0, m_dynamicMaterial);
}
