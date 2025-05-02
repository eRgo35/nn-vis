

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Neuron.generated.h"

UCLASS()
class NNVISUALIZER_API ANeuron : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANeuron();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* m_cubeMesh;
	UMaterialInstanceDynamic* m_dynamicMaterial = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void setActivation(float new_activation);

	void ChangeMeshColor(FLinearColor NewColor);
	
	
};
