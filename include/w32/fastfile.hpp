#pragma once

#include "common.hpp"

#include <string>
#include <vector>

namespace w32
{
	class FastFile
	{
	public:
		using UniqueHandle = w32::UniquePtrHolder<HANDLE, decltype(&::CloseHandle), &::CloseHandle>;
		using Byte = BYTE;

	private:
		UniqueHandle fileHandle{ nullptr };

	public:
		FastFile(const std::wstring & fileName) noexcept
		{
			this->open(fileName);
		}
		~FastFile() noexcept = default;


		[[nodiscard]] constexpr bool isOpen() const noexcept
		{
			return this->fileHandle != nullptr;
		}
		bool open(const std::wstring & fileName) noexcept
		{
			this->fileHandle.reset(::CreateFileW(
				fileName.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ,
				nullptr,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			));
			if (this->fileHandle == INVALID_HANDLE_VALUE)
			{
				this->fileHandle.reset();
				return false;
			}
			else
			{
				return isOpen();
			}
		}
		void close() noexcept
		{
			this->fileHandle.reset();
		}

		std::vector<Byte> readAll() const noexcept
		{
			if (this->fileHandle == nullptr)
				return {};
			
			LARGE_INTEGER li;
			if (!::GetFileSizeEx(this->fileHandle, &li))
				return {};
			
			std::vector<Byte> arr(li.QuadPart);

			if (!::ReadFile(
				this->fileHandle,
				arr.data(),
				li.LowPart,
				nullptr,
				nullptr
			))
			{
				return {};
			}

			return arr;
		}

	};
}
