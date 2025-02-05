#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    // .raw 파일 경로
    std::wstring rawFilePath = L"heightmap.raw";

    // 파일을 바이너리 모드로 열기
    std::ifstream file(rawFilePath,std::ios::binary);
    if(!file)
    {
        std::wcerr << L"Failed to open raw file: " << rawFilePath << std::endl;
        return -1;
    }

    // 파일의 크기 확인 (읽을 데이터 크기 결정)
    file.seekg(0,std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0,std::ios::beg);

    // 예를 들어, 16비트(WORD) 데이터로 구성된 높이맵이라면
    size_t numElements = fileSize / sizeof(WORD);

    // 데이터를 저장할 메모리 할당 (동적 배열)
    WORD* rawData = new WORD[numElements];

    // 파일에서 데이터를 읽기
    if(!file.read(reinterpret_cast<char*>(rawData),fileSize))
    {
        std::wcerr << L"Error reading the raw file." << std::endl;
        delete[] rawData;
        return -1;
    }

    file.close();

    // 데이터를 잘 읽었는지 일부 출력 (디버깅용)
    std::wcout << L"First 10 values from the raw file:" << std::endl;
    for(size_t i = 0; i < 10 && i < numElements; i++)
    {
        std::wcout << rawData[i] << L" ";
    }
    std::wcout << std::endl;

    // 사용 후 메모리 해제
    delete[] rawData;
    return 0;
}