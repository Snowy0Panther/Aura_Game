// Copyright Snowy Panther Inc

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/** Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightingActor() override;
	
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;
	/** end Enemy Interface */

protected:
	virtual void BeginPlay() override;

};
