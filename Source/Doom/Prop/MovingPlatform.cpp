// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Components/TimelineComponent.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

    for (int32 i = 0; i < 3; i++)
    {
        UStaticMeshComponent* GeneratorLight = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("GeneratorLight_%d"), i));
        GeneratorLight->SetupAttachment(movingMesh);
        generatorLights.Add(GeneratorLight);
    }
}



void AMovingPlatform::activateGenerators()
{
    if (activatedGenerators < 3 && greenlightMaterial) {
        generatorLights[activatedGenerators]->SetMaterial(2, greenlightMaterial);
        activatedGenerators++;
    }
}

void AMovingPlatform::interact(FString interactedComponentName, AActor* interactingActor)
{
    if (interactedComponentName == interactableMesh->GetName()) {
        if (activatedGenerators == 3 && !isActivated) {
            isActivated = true;
            
            if (shouldTranslate)  translateTimeline->Play();
            if (shouldRotate)  rotationTimeline->Play();

            playSuccessSound();
            playMovingPlatformSound();
        
        }
        else if (!errorMessagePlayed && !isActivated){
            errorMessagePlayed = true;
            playErrorSound();
            
            FTimerHandle errorMessageTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(errorMessageTimerHandle, [&]()
                {
                    errorMessagePlayed = false;
                }, 10, false);
            
        
        
        }
    
    
    
    
    }
}


