

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LineActor.generated.h"

UCLASS()
class NNVISUALIZER_API ALineActor : public AActor
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_lineMesh;
	UMaterialInstanceDynamic* m_materialInstance = nullptr;
public:	
	// Sets default values for this actor's properties
	ALineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWeight(float weight);
	void ChangeColor(FLinearColor color);
	
};
