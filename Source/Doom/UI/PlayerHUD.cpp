// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Doom/DoomCharacter.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Engine/Texture.h"
#include "TimerManager.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"



void UPlayerHUD::NativeConstruct()
{
    Super::NativeConstruct();
    playerCharacter = Cast<ADoomCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
    InitlizePlayerHUD();
    UpdateCurrentHealth();
}

void UPlayerHUD::NativeDestruct()
{
    Super::NativeDestruct();
    GetWorld()->GetTimerManager().ClearTimer(faceUpdateEvent);
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    updateStaminaBar();
    handleCrosshair();

}

void UPlayerHUD::updateStaminaBar()
{
    if (!playerCharacter)
        return;

    staminaProgressBar->SetPercent(playerCharacter->stamina / playerCharacter->maxStamina);
}

void UPlayerHUD::InitlizePlayerHUD() {
    //Ammo
    maxBulletNo->SetText(FText::AsNumber(playerCharacter->getMaxBullet()));
    maxShellNo->SetText(FText::AsNumber(playerCharacter->getMaxShell()));
    maxRocketNo->SetText(FText::AsNumber(playerCharacter->getMaxRocket())); 
    maxCellNo->SetText(FText::AsNumber(playerCharacter->getMaxCell()));

    UpdateBullet();
    UpdateShell();
    UpdateRocket();
    UpdateCell();

    //Health and Shield
    CurrentHealth->SetText(FText::AsNumber(playerCharacter->getCurHealth()));
    CurrentShield->SetText(FText::AsNumber(playerCharacter->getCurShield()));
}

void UPlayerHUD::UpdateBullet() {
    bulletNo->SetText(FText::AsNumber(playerCharacter->getBullet()));
    CurrentAmmo->SetText(FText::AsNumber(playerCharacter->getBullet()));
}

void UPlayerHUD::UpdateBulletRight()
{
    bulletNo->SetText(FText::AsNumber(playerCharacter->getBullet()));
}

void UPlayerHUD::UpdateShell() {
    shellNo->SetText(FText::AsNumber(playerCharacter->getShell()));
    CurrentAmmo->SetText(FText::AsNumber(playerCharacter->getShell()));
}

void UPlayerHUD::UpdateShellRight()
{
    shellNo->SetText(FText::AsNumber(playerCharacter->getShell()));
}

void UPlayerHUD::UpdateRocket() {
    rocketNo->SetText(FText::AsNumber(playerCharacter->getRocket()));
    CurrentAmmo->SetText(FText::AsNumber(playerCharacter->getRocket()));
}

void UPlayerHUD::UpdateRocketRight()
{
    rocketNo->SetText(FText::AsNumber(playerCharacter->getRocket()));
}

void UPlayerHUD::UpdateCell() {
    cellNo->SetText(FText::AsNumber(playerCharacter->getCell()));
    CurrentAmmo->SetText(FText::AsNumber(playerCharacter->getCell()));
}

void UPlayerHUD::UpdateCellRight()
{
    cellNo->SetText(FText::AsNumber(playerCharacter->getCell()));
}

void UPlayerHUD::UpdateChainsaw()
{
    CurrentAmmo->SetText(FText::FromString("Melee"));
}

void UPlayerHUD::UpdateCurrentHealth()
{
    CurrentHealth->SetText(FText::AsNumber(playerCharacter->getCurHealth()));

    //update face
    float playerHealth = playerCharacter->getCurHealth();
    if (playerHealth <= 100 && playerHealth >= 90) {
        curFace = Face1;
        curFaceTextureIndex = 1;  
    }
    else if (playerHealth < 90 && playerHealth >= 75) {
        curFace = Face2;
        curFaceTextureIndex = 4;
    }
    else if (playerHealth < 75 && playerHealth >= 50) {
        curFace = Face3;
        curFaceTextureIndex = 7;
    }
    else if (playerHealth < 50 && playerHealth >= 25) {
        curFace = Face4;
        curFaceTextureIndex = 10;
    }
    else if (playerHealth < 25 && playerHealth >= 0) {
        curFace = Face5;
        curFaceTextureIndex = 13;
    }

    GetWorld()->GetTimerManager().ClearTimer(faceUpdateEvent);
    GetWorld()->GetTimerManager().SetTimer(faceUpdateEvent, this, &UPlayerHUD::updateFaceImage, 3, true, 0);
}

void UPlayerHUD::UpdateCurrentShield()
{
    CurrentShield->SetText(FText::AsNumber(playerCharacter->getCurShield()));
}

void UPlayerHUD::ActiveRedKey()
{
    RedKey->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerHUD::ActiveOrangeKey()
{
    OrangeKey->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerHUD::ActiveBlueKey()
{
    BlueKey->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerHUD::updateFaceImage()
{
    if (faceTextures.Num() < 14) return;

    switch (curFace) {
    case Face1 :
        switch (curFaceTextureIndex)
        {
        case 0:
            setFaceBrush(1);
            break;

        case 1:
            setFaceBrush(0);
            break;

        default:
            break;
        }
        break;

    case Face2:
        switch (curFaceTextureIndex)
        {
        case 2:
            setFaceBrush(3);
            break;

        case 3:
            setFaceBrush(4);
            break;

        case 4:
            setFaceBrush(2);
            break;

        default:
            break;
        }
        break;
    
    case Face3:
        switch (curFaceTextureIndex)
        {
        case 5:
            setFaceBrush(6);
            break;

        case 6:
            setFaceBrush(7);
            break;

        case 7:
            setFaceBrush(5);
            break;

        default:
            break;
        }
        break;

    case Face4:
        switch (curFaceTextureIndex)
        {
        case 8:
            setFaceBrush(9);
            break;

        case 9:
            setFaceBrush(10);
            break;

        case 10:
            setFaceBrush(8);
            break;

        default:
            break;
        }
        break;

    case Face5:
        switch (curFaceTextureIndex)
        {
        case 11:
            setFaceBrush(12);
            break;

        case 12:
            setFaceBrush(13);
            break;

        case 13:
            setFaceBrush(11);
            break;

        default:
            break;
        }
        break;
    }
}

void UPlayerHUD::setFaceBrush(int32 textureIndex)
{
    curFaceTextureIndex = textureIndex;
    PlayerFace->SetBrushFromTexture(faceTextures[curFaceTextureIndex], true);
}

void UPlayerHUD::handleCrosshair()
{
    UCanvasPanelSlot* bottomCrossCanvasSlot = Cast<UCanvasPanelSlot>(BottomCrosshair->Slot);
    bottomCrossCanvasSlot->SetSize(FVector2D(crosshairThickness, crosshairLength));
    bottomCrossCanvasSlot->SetPosition(FVector2D(-crosshairThickness/2, crosshairSpread));

    UCanvasPanelSlot* topCrossCanvasSlot = Cast<UCanvasPanelSlot>(TopCrosshair->Slot);
    topCrossCanvasSlot->SetSize(FVector2D(crosshairThickness, crosshairLength));
    topCrossCanvasSlot->SetPosition(FVector2D(-crosshairThickness / 2, -crosshairLength - crosshairSpread));

    UCanvasPanelSlot* leftCrossCanvasSlot = Cast<UCanvasPanelSlot>(LeftCrosshair->Slot);
    leftCrossCanvasSlot->SetSize(FVector2D(crosshairLength, crosshairThickness));
    leftCrossCanvasSlot->SetPosition(FVector2D(- crosshairLength - crosshairSpread, - crosshairThickness / 2));

    UCanvasPanelSlot* rightCrossCanvasSlot = Cast<UCanvasPanelSlot>(RightCrosshair->Slot);
    rightCrossCanvasSlot->SetSize(FVector2D(crosshairLength, crosshairThickness));
    rightCrossCanvasSlot->SetPosition(FVector2D(crosshairLength + crosshairSpread - crosshairLength, -crosshairThickness / 2));


}


