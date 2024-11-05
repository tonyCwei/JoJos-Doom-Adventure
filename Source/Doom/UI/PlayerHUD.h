// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
class UBorder;


UENUM(BlueprintType)
enum Face {
	Face1 UMETA(DisplayName = "Face1"),
	Face2 UMETA(DisplayName = "Face2"),
	Face3 UMETA(DisplayName = "Face3"),
	Face4 UMETA(DisplayName = "Face4"),
	Face5 UMETA(DisplayName = "Face5"),
	
};

UCLASS()
class DOOM_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	//native construct needed
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	//player reference
	class ADoomCharacter* playerCharacter;

	//Ammo Text Blocks
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* bulletNo;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxBulletNo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* shellNo;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxShellNo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* rocketNo;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxRocketNo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* cellNo;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* maxCellNo;

	//Stamina
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* staminaProgressBar;

	void updateStaminaBar();

	//Health and Shield
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentShield;

	//KeyCards
	UPROPERTY(meta = (BindWidget))
	UImage* RedKey;

	UPROPERTY(meta = (BindWidget))
	UImage* OrangeKey;

	UPROPERTY(meta = (BindWidget))
	UImage* BlueKey;

	//Player Face


	UPROPERTY(meta = (BindWidget))
	UImage* PlayerFace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<Face> curFace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	TArray<class UTexture2D*> faceTextures;

	int32 curFaceTextureIndex = 0;

	FTimerHandle faceUpdateEvent;


public:

	void InitlizePlayerHUD();

	void UpdateBullet();

	void UpdateBulletRight();

	void UpdateShell();

	void UpdateShellRight();

	void UpdateRocket();

	void UpdateRocketRight();

	void UpdateCell();

	void UpdateCellRight();

	void UpdateChainsaw();

	void UpdateCurrentHealth();

	void UpdateCurrentShield();

	void ActiveRedKey() ;

	void ActiveOrangeKey();

	void ActiveBlueKey();

	void updateFaceImage();

	void setFaceBrush(int32 textureIndex);

//crosshair
protected:
	UPROPERTY(meta = (BindWidget))
	UBorder* TopCrosshair;

	UPROPERTY(meta = (BindWidget))
	UBorder* BottomCrosshair;

	UPROPERTY(meta = (BindWidget))
	UBorder* LeftCrosshair;

	UPROPERTY(meta = (BindWidget))
	UBorder* RightCrosshair;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float crosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float crosshairThickness = 2.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Edits", meta = (AllowPrivateAccess = "true"))
	float crosshairLength = 6;

	void handleCrosshair();

public:

	UFUNCTION(BlueprintCallable)
	void setCrosshairSpread(float crosshairSpread_) { crosshairSpread = crosshairSpread_; }
};
