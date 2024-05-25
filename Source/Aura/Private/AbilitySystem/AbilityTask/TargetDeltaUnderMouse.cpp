// Copyright Snowy Panther Inc


#include "AbilitySystem/AbilityTask/TargetDeltaUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDeltaUnderMouse* UTargetDeltaUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    UTargetDeltaUnderMouse* MyObj = NewAbilityTask<UTargetDeltaUnderMouse>(OwningAbility);
    return MyObj;
}

void UTargetDeltaUnderMouse::Activate()
{
    const bool bIsLocalyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    if (bIsLocalyControlled)
    {
        SendMouseCursorData();
    }
    else
    {
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivatePredictionKey = GetActivationPredictionKey();
        AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivatePredictionKey).AddUObject(this, &UTargetDeltaUnderMouse::OnTargetDataReplicatedCallback);
        const bool bCalledDeligate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivatePredictionKey);
        if (!bCalledDeligate)
        {
            SetWaitingOnRemotePlayerData();
        }
    }
}

void UTargetDeltaUnderMouse::SendMouseCursorData()
{
    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    FHitResult CursorHit;
    PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);

    FGameplayAbilityTargetDataHandle DataHandle;
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    Data->HitResult = CursorHit;
    DataHandle.Add(Data);

    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(), 
        GetActivationPredictionKey(), 
        DataHandle, FGameplayTag(), 
        AbilitySystemComponent->ScopedPredictionKey);

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}

void UTargetDeltaUnderMouse::OnTargetDataReplicatedCallback(
    const FGameplayAbilityTargetDataHandle& DataHandle, 
    FGameplayTag ActivationTag)
{
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}
 