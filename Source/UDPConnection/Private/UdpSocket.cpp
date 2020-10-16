#include "UdpSocket.h"
#include "Engine.h"
#include "SocketSubsystem.h"
#include "UdpSocketBuilder.h"
#include "UdpSocketReceiver.h"
#include "UnrealString.h"

AUdpSocket::AUdpSocket(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SenderSocket = NULL;
	ListenSocket = NULL;
}

#pragma region SenderImplemention
bool AUdpSocket::StartUDPSender(const FString& SocketName, const FString& IPAddr, const int32 Port)
{
	//Create Remote Address.
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*IPAddr, bIsValid);
	RemoteAddr->SetPort(Port);

	SenderSocket = FUdpSocketBuilder(*SocketName).AsReusable().WithBroadcast();

	//Set Send Buffer Size
	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	return true;
}


bool AUdpSocket::UdpSocket_SendString(FString ToSend)
{

	if (!SenderSocket && GEngine) {
		GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Sender socket not found!");
		return false;
	}

	uint8* bytesToSend = new uint8[ToSend.Len()];
	int32 bytesSent;

	TCHAR* SendData = ToSend.GetCharArray().GetData();
	int32 DataLen = FCString::Strlen(SendData);
	uint8* ByteData = (uint8*)TCHAR_TO_UTF8(SendData);

	SenderSocket->SendTo(ByteData, DataLen, bytesSent, *RemoteAddr);

	//Debug purposes
	//GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("bytesSent: %d"), bytesSent));

	return true;
}
#pragma endregion


#pragma region ReceiverImplemention

void AUdpSocket::ReceiveData(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	FString ReceivedData = "";
	TArray<uint8> RawData;
	int32 bytesRead = 0;
	RawData.AddUninitialized(ArrayReaderPtr->TotalSize());
	ArrayReaderPtr->Serialize(RawData.GetData(), ArrayReaderPtr->TotalSize());
	char utfData[1024];
	FMemory::Memcpy(utfData, RawData.GetData(), bytesRead);
	utfData[bytesRead] = 0;
	FString debugData = UTF8_TO_TCHAR(utfData);
	BPEvent_DataReceived(debugData);
}

bool AUdpSocket::StartUDPReceiver(const FString& SocketName, const int32 Port, int32 WaitTimeInMilis)
{
	FIPv4Address Addr = FIPv4Address::Any;

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, Port);

	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;

	ListenSocket = FUdpSocketBuilder(*SocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize)
		.BoundToPort(Port);

	ListenSocket->SetSendBufferSize(BufferSize, BufferSize);
	ListenSocket->SetReceiveBufferSize(BufferSize, BufferSize);

	try {
		FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(WaitTimeInMilis);
		UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP_RECEIVER"));
		UDPReceiver->OnDataReceived().BindUObject(this, &AUdpSocket::ReceiveData);
		UDPReceiver->Start();
		return true;
	}
	catch (FString message) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Red, message);
		}
		return false;
	}
}
#pragma endregion


#pragma region ActorImplemetion
void AUdpSocket::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//UDPReceiver->Stop();
	delete UDPReceiver;
	UDPReceiver = nullptr;

	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
	}

	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}
#pragma endregion

FString AUdpSocket::BytesToStringFixed(const uint8* In, int32 Count)
{
	FString Broken = BytesToString(In, Count);
	FString Fixed;

	for (int i = 0; i < Broken.Len(); i++)
	{
		const TCHAR c = Broken[i] - 1;
		Fixed.AppendChar(c);
	}

	return Fixed;
}