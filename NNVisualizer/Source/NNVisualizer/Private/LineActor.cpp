


#include "LineActor.h"


// Sets default values
ALineActor::ALineActor()
{
	m_lineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineMesh"));
	RootComponent = m_lineMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		m_lineMesh->SetStaticMesh(CubeMesh.Object);
		m_lineMesh->SetCastShadow(false);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial(TEXT("/Game/DynMat"));
	if (TranslucentMaterial.Succeeded())
	{
		m_lineMesh->SetMaterial(0, TranslucentMaterial.Object);
		m_materialInstance = UMaterialInstanceDynamic::Create(m_lineMesh->GetMaterial(0), this);
	}
}
	


// Called when the game starts or when spawned
void ALineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALineActor::SetWeight(float weight)
{
	ChangeColor(FLinearColor(-weight, 0, weight));
}

void ALineActor::ChangeColor(FLinearColor color)
{
	if (m_materialInstance == nullptr)
	{
		m_materialInstance->SetVectorParameterValue(FName("Color"), color);
		m_lineMesh->SetMaterial(0, m_materialInstance);
	}
	m_materialInstance->SetVectorParameterValue("Color", color);
}