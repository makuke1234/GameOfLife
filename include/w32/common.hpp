#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WIN32_EXTRA_LEAN
	#define WIN32_EXTRA_LEAN
#endif

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include <Windows.h>

#include <memory>
#include <type_traits>
#include <cstdint>
#include <atomic>

namespace w32
{
	[[nodiscard]] constexpr COLORREF BGR(BYTE blue, BYTE green, BYTE red) noexcept
	{
		return RGB(red, green, blue);
	}

	template<class PT, class Deleter, Deleter deleter>
	class UniquePtrHolder
	{
	private:
		PT m_ptr{};

	public:
		UniquePtrHolder() noexcept = default;
		explicit constexpr UniquePtrHolder(PT ptr) noexcept
			: m_ptr{ ptr }
		{}
		UniquePtrHolder(const UniquePtrHolder & other) = delete;
		constexpr UniquePtrHolder(UniquePtrHolder && other) noexcept
			: m_ptr{ other.m_ptr }
		{
			other.m_ptr = 0;
		}
		template<class U, class E, E e>
		explicit constexpr UniquePtrHolder(UniquePtrHolder<U, E, e> && other) noexcept
			: m_ptr{ static_cast<PT>(other.m_ptr) }
		{
			other.m_ptr = 0;
		}
		~UniquePtrHolder() noexcept
		{
			if (this->m_ptr != 0)
			{
				deleter(this->m_ptr);
				this->m_ptr = 0;
			}
		}

		UniquePtrHolder & operator=(const UniquePtrHolder & other) = delete;
		UniquePtrHolder & operator=(UniquePtrHolder && other) noexcept
		{
			this->~UniquePtrHolder();
			this->m_ptr = other.m_ptr;
			other.m_ptr = 0;

			return *this;
		}
		template<class U, class E, E e>
		UniquePtrHolder & operator=(UniquePtrHolder<U, E, e> && other) noexcept
		{
			this->~UniquePtrHolder();
			this->m_ptr = static_cast<PT>(other.m_ptr);
			other.m_ptr = 0;

			return *this;
		}


		[[nodiscard]] constexpr PT release() noexcept
		{
			auto ret{ this->m_ptr };
			this->m_ptr = 0;
			return ret;
		}
		void reset(PT ptr = PT()) noexcept
		{
			this->~UniquePtrHolder();
			this->m_ptr = ptr;
		}
		void swap(UniquePtrHolder & other) noexcept
		{
			auto temp{ std::move(other) };
			other = std::move(*this);
			*this = std::move(temp);
		}


		[[nodiscard]] constexpr operator PT() const noexcept
		{
			return this->m_ptr;
		}
		template<class Y>
		[[nodiscard]] constexpr Y get() noexcept
		{
			return this->m_ptr;
		}
		[[nodiscard]] constexpr PT get() const noexcept
		{
			return this->m_ptr;
		}
		[[nodiscard]] constexpr Deleter & get_deleter() noexcept
		{
			return deleter;
		}
		[[nodiscard]] constexpr const Deleter & get_deleter() const noexcept
		{
			return deleter;
		}
		[[nodiscard]] explicit constexpr operator bool() const noexcept
		{
			return this->m_ptr != 0;
		}
	};

	template<class PT>
	class WeakPtrHolder;

	class SharedPtrControlBlock
	{
	private:
		std::atomic_uint32_t m_useCount{ 0 };

	public:
		constexpr SharedPtrControlBlock(std::uint32_t i = 0) noexcept
			: m_useCount(i)
		{}

		void inc() noexcept
		{
			++this->m_useCount;
		}
		void dec() noexcept
		{
			--this->m_useCount;
		}
		[[nodiscard]] std::uint32_t dec_get() noexcept
		{
			return --this->m_useCount;
		}
		[[nodiscard]] std::uint32_t get() const noexcept
		{
			return this->m_useCount;
		}
	};

	template<class PT, class Deleter, Deleter deleter>
	class SharedPtrHolder
	{
	private:
		friend class WeakPtrHolder<PT>;

		PT m_ptr{};
		SharedPtrControlBlock * m_control{ nullptr };

	public:
		constexpr SharedPtrHolder() noexcept = default;
		template<class Y>
		explicit SharedPtrHolder(Y ptr) noexcept
			: m_ptr{ static_cast<PT>(ptr) }, m_control{ new SharedPtrControlBlock{ 1 } }
		{}
		template<class Y, class ODeleter, ODeleter oDeleter>
		constexpr SharedPtrHolder(const SharedPtrHolder<Y, ODeleter, oDeleter> & other, PT ptr) noexcept
			: m_ptr{ ptr }, m_control{ other.m_control }
		{
			this->m_control->inc();
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		constexpr SharedPtrHolder(SharedPtrHolder<Y, ODeleter, oDeleter> && other, PT ptr) noexcept
			: m_ptr{ ptr }, m_control{ other.m_control }
		{
			other.m_ptr = 0;
			other.m_control = nullptr;
		}
		constexpr SharedPtrHolder(const SharedPtrHolder & other) noexcept
			: m_ptr{ other.m_ptr }, m_control{ other.m_control }
		{
			this->m_control->inc();
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		constexpr SharedPtrHolder(const SharedPtrHolder<Y, ODeleter, oDeleter> & other) noexcept
			: m_ptr{ static_cast<PT>(other.m_ptr) }, m_control{ other.m_control }
		{
			this->m_control->inc();
		}
		constexpr SharedPtrHolder(SharedPtrHolder && other) noexcept
			: m_ptr{ other.m_ptr }, m_control{ other.m_control }
		{
			other.m_ptr = 0;
			other.m_control = nullptr;
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		constexpr SharedPtrHolder(SharedPtrHolder<Y, ODeleter, oDeleter> && other) noexcept
			: m_ptr{ static_cast<PT>(other.m_ptr) }, m_control{ other.m_control }
		{
			other.m_ptr = 0;
			other.m_control = nullptr;
		}
		template<class Y>
		explicit SharedPtrHolder(const WeakPtrHolder<Y> & other)
			: m_ptr{ static_cast<PT>(other.get()) }, m_control{ new SharedPtrControlBlock{ 1 } }
		{}
		template<class Y, class ODeleter, ODeleter oDeleter>
		SharedPtrHolder(UniquePtrHolder<Y, ODeleter, oDeleter> && other)
			: m_ptr{ static_cast<PT>(other.get()) }, m_control{ new SharedPtrControlBlock{ 1 } }
		{
			other.m_ptr = 0;
		}
		~SharedPtrHolder()
		{
			if (this->m_ptr != 0)
			{
				deleter(this->m_ptr);
				this->m_ptr = 0;
			}
			if (this->m_control != nullptr)
			{
				if (this->m_control->dec_get() == 0)
				{
					delete this->m_control;
					this->m_control = nullptr;
				}
			}
		}


		SharedPtrHolder & operator=(const SharedPtrHolder & other) noexcept
		{
			this->~SharedPtrHolder();
			this->m_ptr = other.m_ptr;
			this->m_control = other.m_control;
			this->m_control->inc();

			return *this;
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		SharedPtrHolder & operator=(const SharedPtrHolder<Y, ODeleter, oDeleter> & other) noexcept
		{
			this->~SharedPtrHolder();
			this->m_ptr = static_cast<PT>(other.m_ptr);
			this->m_control = other.m_control;
			this->m_control->inc();

			return *this;
		}
		SharedPtrHolder & operator=(SharedPtrHolder && other) noexcept
		{
			this->~SharedPtrHolder();
			this->m_ptr = other.m_ptr;
			this->m_control = other.m_control;

			other.m_ptr = 0;
			other.m_control = nullptr;

			return *this;
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		SharedPtrHolder & operator=(SharedPtrHolder<Y, ODeleter, oDeleter> && other) noexcept
		{
			this->~SharedPtrHolder();
			this->m_ptr = static_cast<PT>(other.m_ptr);
			this->m_control = other.m_control;

			other.m_ptr = 0;
			other.m_control = nullptr;
			
			return *this;
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		SharedPtrHolder & operator=(UniquePtrHolder<Y, ODeleter, oDeleter> && other) noexcept
		{
			this->~SharedPtrHolder();
			this->m_ptr = static_cast<PT>(other.m_ptr);
			this->m_control = new SharedPtrControlBlock{ 1 };

			other.m_ptr = 0;

			return *this;
		}

		void reset() noexcept
		{
			this->~SharedPtrHolder();
		}
		template<class Y>
		void reset(Y ptr)
		{
			this->~SharedPtrHolder();
			this->m_ptr = static_cast<PT>(ptr);
			this->m_control = new SharedPtrControlBlock{ 1 };
		}
		void swap(SharedPtrHolder & other) noexcept
		{
			auto temp{ std::move(other) };
			other = std::move(*this);
			*this = std::move(temp);
		}

		template<class Y>
		[[nodiscard]] constexpr Y get() noexcept
		{
			return this->m_ptr;
		}
		[[nodiscard]] constexpr PT get() const noexcept
		{
			return this->m_ptr;
		}

		[[nodiscard]] std::uint32_t use_count() const noexcept
		{
			if (this->m_control == nullptr)
				return 0;
			else
				return this->m_control->get();
		}
		[[nodiscard]] explicit constexpr operator bool() const noexcept
		{
			return this->m_ptr != 0;
		}
		template<class Y, class ODeleter, ODeleter oDeleter>
		[[nodiscard]] constexpr bool owner_before(const SharedPtrHolder<Y, ODeleter, oDeleter> & other) const noexcept
		{
			return this->m_control == other.m_control;
		}
		template<class Y>
		[[nodiscard]] constexpr bool owner_before(const WeakPtrHolder<Y> & other) const noexcept
		{
			return this->m_control == other.m_control;
		}
	};
	
	template<class PT>
	class WeakPtrHolder
	{
	private:
		static constexpr SharedPtrControlBlock c_nullBlock{};

		PT m_ptr{};
		const SharedPtrControlBlock * m_control{ &c_nullBlock };

	public:
		constexpr WeakPtrHolder() noexcept = default;
		WeakPtrHolder(const WeakPtrHolder & other) noexcept = default;
		template<class Y>
		constexpr WeakPtrHolder(const WeakPtrHolder<Y> & other) noexcept
			: m_ptr{ static_cast<PT>(other.m_ptr) }, m_control{ other.m_control }
		{}
		template<class Y, class Deleter, Deleter deleter>
		WeakPtrHolder(const w32::SharedPtrHolder<Y, Deleter, deleter> & other) noexcept
			: m_ptr{ static_cast<PT>(other) }, m_control{ other.m_control }
		{}
		constexpr WeakPtrHolder(WeakPtrHolder && other) noexcept
			: m_ptr{ other.m_ptr }, m_control{ other.m_control }
		{
			other.m_ptr = 0;
			other.m_control = nullptr;
		}
		template<class Y>
		constexpr WeakPtrHolder(WeakPtrHolder<Y> && other) noexcept
			: m_ptr{ static_cast<PT>(other.m_ptr) }, m_control{ other.m_control }
		{
			other.m_ptr = 0;
			other.m_control = nullptr;
		}
		~WeakPtrHolder() noexcept = default;

		constexpr WeakPtrHolder & operator=(const WeakPtrHolder & other) noexcept
		{
			this->~WeakPtrHolder();
			this->m_ptr = other.m_ptr;
			this->m_control = other.m_control;

			return *this;
		}
		template<class Y>
		constexpr WeakPtrHolder & operator=(const WeakPtrHolder<Y> & other) noexcept
		{
			this->~WeakPtrHolder();
			this->m_ptr = static_cast<PT>(other.m_ptr);
			this->m_control = other.m_control;

			return *this;
		}
		template<class Y, class Deleter, Deleter deleter>
		constexpr WeakPtrHolder & operator=(const SharedPtrHolder<Y, Deleter, deleter> & other) noexcept
		{
			this->~WeakPtrHolder();
			this->m_ptr = static_cast<PT>(other.get());
			this->m_control = other.m_control;

			return *this;
		}
		constexpr WeakPtrHolder & operator=(WeakPtrHolder && other) noexcept
		{
			this->m_ptr = other.m_ptr;
			this->m_control = other.m_control;

			other.m_ptr = 0;
			other.m_control = nullptr;

			return *this;
		}
		template<class Y>
		constexpr WeakPtrHolder & operator=(WeakPtrHolder<Y> && other) noexcept
		{
			this->m_ptr = static_cast<PT>(other.m_ptr);
			this->m_control = other.m_control;

			other.m_ptr = 0;
			other.m_control = nullptr;

			return *this;
		}

		constexpr void reset() noexcept
		{
			*this = WeakPtrHolder();
		}
		constexpr void swap(WeakPtrHolder & other) noexcept
		{
			auto temp{ std::move(other) };
			other = std::move(*this);
			*this = std::move(temp);
		}

		[[nodiscard]] std::uint32_t use_count() const noexcept
		{
			return this->m_control->get();
		}
		[[nodiscard]] bool expired() const noexcept
		{
			return this->use_count() == 0;
		}
		template<class Deleter, Deleter deleter>
		[[nodiscard]] SharedPtrHolder<PT, Deleter, deleter> lock() const noexcept
		{
			return { *this };
		}

		template<class Y>
		[[nodiscard]] owner_before(const WeakPtrHolder<Y> & other) const noexcept
		{
			return this->m_control == other.m_control;
		}
		template<class Y, class Deleter, Deleter deleter>
		[[nodiscard]] owner_before(const SharedPtrHolder<Y, Deleter, deleter> & other) const noexcept
		{
			return this->m_control == other.m_control;
		}

	};

	using UniqueWnd = w32::UniquePtrHolder<::HWND, decltype(&::DestroyWindow), &::DestroyWindow>;
	using SharedWnd = w32::SharedPtrHolder<::HWND, decltype(&::DestroyWindow), &::DestroyWindow>;
	using WeakWnd   = w32::WeakPtrHolder<::HWND>;

}
