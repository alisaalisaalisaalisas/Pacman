#pragma once

#include <SFML/Window/Event.hpp>

#include <string>
#include <vector>

class Menu {
public:
    void setTitle(std::string title) { mTitle = std::move(title); }
    void setItems(std::vector<std::string> items);

    void moveUp();
    void moveDown();

    // Returns true when the user activates an item.
    bool handleEvent(const sf::Event& e);

    const std::string& title() const { return mTitle; }
    const std::vector<std::string>& items() const { return mItems; }
    std::size_t selectedIndex() const { return mSelected; }
    void setSelected(std::size_t idx) { if (idx < mItems.size()) mSelected = idx; }

private:
    std::string mTitle;
    std::vector<std::string> mItems;
    std::size_t mSelected = 0;
};
