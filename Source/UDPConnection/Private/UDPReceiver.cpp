// Fill out your copyright notice in the Description page of Project Settings.



#include "UDPReceiver.h"
#include "IPv4Address.h"
#include "IPv4Endpoint.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "UdpSocketBuilder.h"


// Sets default values
AUDPReceiver::AUDPReceiver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ListenSocket = NULL;
}



void AUDPReceiver::Recv(FString &outStr, bool &outSuccess)
{
	
	if (ListenSocket) {
		TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		TArray<uint8> ReceivedData;
		uint32 Size;
		if (ListenSocket->HasPendingData(Size)) {
			outSuccess = true;
			outStr = "";
			uint8* RecvData = new uint8[Size];
			int32 bytesRead = 0;

			ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
			ListenSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), bytesRead, *targetAddr);
			char utfData[1024];
			FMemory::Memcpy(utfData, ReceivedData.GetData(), bytesRead);
			utfData[bytesRead] = 0;
			FString debugData = UTF8_TO_TCHAR(utfData);
			ReadData = outStr = debugData;
		}
		else {
			outSuccess = false;
			ReadData = "";
		}
	}
	else {
		outSuccess = false;
		ReadData = "";
	}
}

bool AUDPReceiver::StartUDPReceiver(const FString&SocketName, const FString& IP, const int32 Port)
{
	FIPv4Address Addr;
	FIPv4Address::Parse(IP, Addr);

	//Create Socket
	FIPv4Endpoint EndPoint(FIPv4Address::Any, Port);

	//Buffer Size
	int32 BufferSize = 2 * 1024 * 1024;

	ListenSocket = FUdpSocketBuilder(*SocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(EndPoint)
		.WithReceiveBufferSize(BufferSize)
		.BoundToPort(Port)
		;

	ListenSocket->SetSendBufferSize(BufferSize, BufferSize);
	ListenSocket->SetReceiveBufferSize(BufferSize, BufferSize);

	return true;
}

void AUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UDPReceiver!=nullptr)
	{
		//UDPReceiver->Stop();
		UDPReceiver->Exit();
		delete UDPReceiver;
		UDPReceiver = nullptr;
	}


	//clearing sockets
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

// Called when the game starts or when spawned
void AUDPReceiver::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUDPReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Recv(ReadData, readSuccess);

	if (ListenSocket!=nullptr)
	{
		switch (ListenSocket->GetConnectionState())
		{
		case ESocketConnectionState::SCS_Connected:
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, "Connected"+ListenSocket->GetProtocol().ToString() );
			break;

		case ESocketConnectionState::SCS_ConnectionError:
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, "ConnectionError");
			break;

		case ESocketConnectionState::SCS_NotConnected:
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, "Not Connected");
			break;

		default:
			break;
		}
		
	}
}

