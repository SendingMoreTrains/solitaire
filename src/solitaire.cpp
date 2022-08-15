enum Suit {
    Spades = 0,
    Diamonds = 1,
    Clubs = 2,
    Hearts = 3
};

enum CardColor {
    Black,
    Red
};

struct Card {
    Card(SpriteSheet* src_sheet, Suit suit, int rank);

    CardColor getColor();

    void setPosition(int x, int y);
    void render(RenderContext* rc);

    Suit suit;
    int rank;
    Sprite sprite;
    rect area;
};

Card::Card(SpriteSheet* src_sheet, Suit suit, int rank)
        : suit{suit}, rank{rank}, sprite{src_sheet->createSprite(suit, rank - 1)},
          area{0, 0, sprite.src_rect.w, sprite.src_rect.h} {}

CardColor Card::getColor() {
    if (suit == Suit::Spades || suit == Suit::Clubs) {
        return CardColor::Black;
    }

    return CardColor::Red;
}

void Card::setPosition(int x, int y) {
    area.x = x;
    area.y = y;
}

void Card::render(RenderContext* rc) {
    rc->renderSprite(sprite, &area);
}


std::vector<Card> cards{};
SpriteSheet* spriteSheet = nullptr;


void init(RenderContext* renderContext) {
    spriteSheet = new SpriteSheet(renderContext->loadTexture("res/card_spritesheet.png"), 32, 48);

    cards.reserve(10);
    for (int i = 0; i < 10; ++i) {
        cards.emplace_back(spriteSheet, static_cast<Suit>(i % 4), i + 2);
        cards.back().setPosition(20, 20 + (i * 13));
    }
}

void update(MouseState* mouseState)
{
    if (mouseState->left.wasReleased) {
        auto x = mouseState->x;
        auto y = mouseState->y;
        std::cout << "X: " << x << "\nY: " << y << std::endl;
        for (auto card_i = cards.rbegin(); card_i != cards.rend(); ++card_i) {
            rect c_area{card_i->area};
            if (x > c_area.x && x < c_area.x + c_area.w && y > c_area.y && y < c_area.y + c_area.h) {
                std::cout << "Rank: " << card_i->rank << ", Suit: " << card_i->suit << "\n" << std::endl;
                break;
            }
        }
    }

}

void render(RenderContext* renderContext) {
    renderContext->setDrawColor(22, 128, 17);
    renderContext->clearScreen();

    for (auto card_i = cards.begin(); card_i != cards.end(); ++card_i) {
        card_i->render(renderContext);
    }

    renderContext->present();
}
