// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "UDPData.h"
#include "Networking.h"
#include "UDPReceiver.generated.h"

UCLASS()
class AUDPReceiver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUDPReceiver();

	UPROPERTY(BlueprintReadOnly)
	FString ReadData="";

	bool readSuccess;

	UPROPERTY(EditDefaultsOnly)
	int32 UDPTaskWaitTimeMiliseconds = 100;


	UFUNCTION(BlueprintImplementableEvent,Category="UDP Read")
	void UDPDataReceived(const FString& ReceivedString, bool &success);

public:
	FSocket* ListenSocket;

	FUdpSocketReceiver* UDPReceiver = nullptr;

	void Recv(FString &outStr, bool &outSuccess);

	UFUNCTION(BlueprintCallable, Category = "UDP")
	bool StartUDPReceiver(const FString&SocketName, const FString& IP, const int32 Port);

public:

	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
