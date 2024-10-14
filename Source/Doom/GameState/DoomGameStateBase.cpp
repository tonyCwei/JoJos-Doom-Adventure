// Fill out your copyright notice in the Description page of Project Settings.


#include "DoomGameStateBase.h"

void ADoomGameStateBase::addAttack(FAttackInfo curAttackInfo)
{
	activeAttacks.Add(curAttackInfo);
}

void ADoomGameStateBase::removeAttack(FAttackInfo curAttackInfo)
{
	activeAttacks.Remove(curAttackInfo);
}
