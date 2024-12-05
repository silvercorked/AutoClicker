
#include <iostream>
#include <windows.h>
#include <chrono>
#include <format>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <utility>

#include "ASCII.hpp"
#include "PrimitiveTypes.hpp"

using namespace std::chrono_literals;

constexpr const bool UntilKillCode = true;

constexpr const u32 MaxClicks = 500;
constexpr const f32 ClicksPerSecond = 20.0f;

const std::string KillCodes = "";

constexpr const u32 StartMouseXPos = 1200;
constexpr const u32 StartMouseYPos = 500;


static const auto timeBetween = std::chrono::duration_cast<std::chrono::microseconds>((1.0s / ClicksPerSecond));

constexpr auto [[nodiscard]] parseKillCodes(std::string) -> const std::vector<i32>;
auto attemptClick(
	std::chrono::steady_clock::time_point&,
	std::chrono::steady_clock::time_point const&,
	std::chrono::microseconds const&
) -> bool;
auto [[nodiscard]] checkIfKeyPressed(i32) -> bool;
auto [[nodiscard]] checkIfAnyKeyPressed(std::vector<i32> const&) -> bool;

auto main() -> int {
	const std::vector<i32> killcodes = parseKillCodes(KillCodes);
	if (UntilKillCode && killcodes.size() == 0) {
		std::cout << "Cannot run forever without kill code! set KillCode Variable or set UntilKillCode to false\n";
		return 0;
	}
	SetCursorPos(StartMouseXPos, StartMouseYPos);
	auto reminderTime = std::chrono::steady_clock::now();
	auto oldTime = std::chrono::steady_clock::now();
	bool clicked = false;
	for (u32 i = 0; UntilKillCode ? true :  i < MaxClicks; i += clicked) {
		if (checkIfAnyKeyPressed(killcodes)) {
			break;
		}
		auto currTime = std::chrono::steady_clock::now();
		clicked = attemptClick(oldTime, currTime, timeBetween);
		if constexpr (!UntilKillCode) {
			if (clicked) {
				std::cout << std::format("clicks remaining: {}, ETA: {}\n",
					MaxClicks - i, std::chrono::duration_cast<std::chrono::seconds>(timeBetween * (MaxClicks - i))
				);
			}
		}
		if (std::chrono::duration_cast<std::chrono::seconds>(currTime - reminderTime) > 1s) {
			std::cout << std::format(
				"REMINDER: {}\n\tkillCodes: {}\n",
				(UntilKillCode ? "Awaiting Kill Code" : "Awaiting Clicks or Kill Code"), KillCodes
			);
		}
	}
	return 0;
}

constexpr auto [[nodiscard]] parseKillCodes(std::string killCodes) -> const std::vector<i32> {
	std::vector<i32> result;
	std::transform(killCodes.cbegin(), killCodes.cend(), killCodes.begin(), [](u8 c) { return std::toupper(c); }); // for some reason, always assumes keys are pressed in upper case, so just set them all to that
	result.append_range(killCodes);
	return result;
}

auto attemptClick(
	std::chrono::steady_clock::time_point& oldTime,
	std::chrono::steady_clock::time_point const& currTime,
	std::chrono::microseconds const& timeBetweenRequired
) -> bool {
	if (std::chrono::duration_cast<std::chrono::microseconds>(currTime - oldTime) < timeBetweenRequired) return false;
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	oldTime = currTime;
	return true;
}
//https://www.geeksforgeeks.org/how-to-detect-keypress-in-windows-using-cpp/
auto [[nodiscard]] checkIfKeyPressed(const int keycode) -> bool {
	if (GetAsyncKeyState(keycode) & 0x8000) {
		std::cout << std::format("Pressed Key: {} (ASCII value: {})\n",
			static_cast<char>(keycode), keycode
		);
		return true;
	}
	return false;
}

auto [[nodiscard]] checkIfAnyKeyPressed(std::vector<int> const& keycodes) -> bool {
	for (auto const& keycode : keycodes) {
		if (checkIfKeyPressed(keycode)) {
			return true;
		}
	}
	return false;
}
