#include "Menu.h"

void Menu::setItems(std::vector<std::string> items) {
    mItems = std::move(items);
    mSelected = 0;
}

void Menu::moveUp() {
    if (mItems.empty()) return;
    if (mSelected == 0) {
        mSelected = mItems.size() - 1;
    } else {
        --mSelected;
    }
}

void Menu::moveDown() {
    if (mItems.empty()) return;
    mSelected = (mSelected + 1) % mItems.size();
}

bool Menu::handleEvent(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) {
        return false;
    }

    switch (e.key.code) {
    case sf::Keyboard::Up:
    case sf::Keyboard::W:
        moveUp();
        return false;
    case sf::Keyboard::Down:
    case sf::Keyboard::S:
        moveDown();
        return false;
    case sf::Keyboard::Enter:
    case sf::Keyboard::Space:
        return true;
    default:
        return false;
    }
}
