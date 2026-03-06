#pragma once

#include <optional>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Board.h"

namespace chess {

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window_;
    Board board_;

    sf::Font font_;

    std::optional<Position> selectedCell_;
    std::vector<Move> highlightedMoves_;

    std::string statusMessage_;

    sf::FloatRect boardRect_;
    sf::FloatRect restartButtonRect_;
    std::vector<std::pair<PieceType, sf::FloatRect>> promotionButtons_;

    void processEvents();
    void updateLayout();
    void render();

    void handleClick(sf::Vector2f position);
    std::optional<Position> screenToBoard(sf::Vector2f position) const;
    sf::Vector2f boardToScreen(Position pos) const;

    void selectCell(Position position);
    void clearSelection();

    void drawBoard(sf::RenderTarget& target);
    void drawPieces(sf::RenderTarget& target);
    void drawStatusPanel(sf::RenderTarget& target);

    void drawPieceGlyph(sf::RenderTarget& target, const Piece& piece, sf::Vector2f center, float size);

    std::string turnLabel() const;
    std::string pieceLabel(const Piece& piece) const;
};

}  // namespace chess
