// Fill out your copyright notice in the Description page of Project Settings.


#include "OrionActor.h"

// Sets default values
AOrionActor::AOrionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

