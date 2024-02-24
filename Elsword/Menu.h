#pragma once

namespace Base
{
	class Menu
	{
	public:
		Menu()  noexcept = default;
		~Menu() noexcept = default;
		Menu(Menu const&) = delete;
		Menu(Menu&&) = delete;
		Menu& operator=(Menu const&) = delete;
		Menu& operator=(Menu&&) = delete;

		// FORWARD DECLARE FUNCTIONS
		void Draw();
		void MainMenu();
	};
	inline std::unique_ptr<Menu> g_Menu;
}