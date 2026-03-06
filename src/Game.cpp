#include "Game.h"

#include <algorithm>

namespace chess {

Game::Game()
    : window_(sf::VideoMode(1100, 760), "Xadrez C++ / SFML", sf::Style::Default) {
    window_.setFramerateLimit(60);
    font_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    updateLayout();
    statusMessage_ = "Bem-vindo! Vez das brancas.";
}

void Game::run() {
    while (window_.isOpen()) {
        processEvents();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        }

        if (event.type == sf::Event::Resized) {
            window_.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                                   static_cast<float>(event.size.height))));
            updateLayout();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            handleClick(window_.mapPixelToCoords(sf::Mouse::getPosition(window_)));
        }
    }
}

void Game::updateLayout() {
    const auto size = window_.getSize();
    const float width = static_cast<float>(size.x);
    const float height = static_cast<float>(size.y);

    const float padding = 20.f;
    const float panelWidth = std::max(240.f, width * 0.25f);
    const float boardAvailableWidth = width - panelWidth - padding * 3.f;
    const float boardSize = std::max(320.f, std::min(boardAvailableWidth, height - padding * 2.f));

    boardRect_ = sf::FloatRect(padding, (height - boardSize) / 2.f, boardSize, boardSize);
    restartButtonRect_ = sf::FloatRect(boardRect_.left + boardRect_.width + padding, padding + 180.f,
                                       panelWidth - padding, 52.f);

    promotionButtons_.clear();
    float buttonY = restartButtonRect_.top + restartButtonRect_.height + 36.f;
    const float buttonH = 44.f;
    for (PieceType type : {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight}) {
        promotionButtons_.push_back({
            type,
            sf::FloatRect(boardRect_.left + boardRect_.width + padding, buttonY, panelWidth - padding, buttonH),
        });
        buttonY += buttonH + 10.f;
    }
}

std::optional<Position> Game::screenToBoard(sf::Vector2f position) const {
    if (!boardRect_.contains(position)) {
        return std::nullopt;
    }

    const float cell = boardRect_.width / 8.f;
    const int col = static_cast<int>((position.x - boardRect_.left) / cell);
    const int row = static_cast<int>((position.y - boardRect_.top) / cell);

    return Position{row, col};
}

sf::Vector2f Game::boardToScreen(Position pos) const {
    const float cell = boardRect_.width / 8.f;
    return sf::Vector2f(boardRect_.left + pos.col * cell, boardRect_.top + pos.row * cell);
}

void Game::handleClick(sf::Vector2f position) {
    if (restartButtonRect_.contains(position)) {
        board_.reset();
        clearSelection();
        statusMessage_ = "Partida reiniciada. Vez das brancas.";
        return;
    }

    if (board_.hasPendingPromotion()) {
        for (const auto& [type, rect] : promotionButtons_) {
            if (rect.contains(position)) {
                board_.promotePawn(type, statusMessage_);
                return;
            }
        }
        statusMessage_ = "Escolha uma peça para promoção.";
        return;
    }

    auto cell = screenToBoard(position);
    if (!cell) {
        clearSelection();
        return;
    }

    if (selectedCell_) {
        auto moveIt = std::find_if(highlightedMoves_.begin(), highlightedMoves_.end(),
                                   [&](const Move& move) { return move.to == *cell; });
        if (moveIt != highlightedMoves_.end()) {
            board_.tryMove(*moveIt, statusMessage_);
            clearSelection();
            return;
        }
    }

    const Piece* piece = board_.getPiece(*cell);
    if (piece && piece->color() == board_.currentTurn()) {
        selectCell(*cell);
    } else {
        clearSelection();
        statusMessage_ = "Selecione uma peça válida.";
    }
}

void Game::selectCell(Position position) {
    selectedCell_ = position;
    highlightedMoves_ = board_.getLegalMoves(position);

    const Piece* piece = board_.getPiece(position);
    if (piece) {
        statusMessage_ = "Peça selecionada: " + pieceLabel(*piece);
    }
}

void Game::clearSelection() {
    selectedCell_.reset();
    highlightedMoves_.clear();
}

void Game::render() {
    window_.clear(sf::Color(22, 24, 30));

    drawBoard(window_);
    drawPieces(window_);
    drawStatusPanel(window_);

    window_.display();
}

void Game::drawBoard(sf::RenderTarget& target) {
    const float cell = boardRect_.width / 8.f;

    sf::RectangleShape bg(sf::Vector2f(boardRect_.width + 12.f, boardRect_.height + 12.f));
    bg.setPosition(boardRect_.left - 6.f, boardRect_.top - 6.f);
    bg.setFillColor(sf::Color(35, 38, 47));
    target.draw(bg);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            sf::RectangleShape cellShape(sf::Vector2f(cell, cell));
            cellShape.setPosition(boardRect_.left + col * cell, boardRect_.top + row * cell);

            bool light = (row + col) % 2 == 0;
            cellShape.setFillColor(light ? sf::Color(240, 217, 181) : sf::Color(181, 136, 99));
            target.draw(cellShape);
        }
    }

    if (board_.isInCheck(PieceColor::White)) {
        Position king = {7, 4};
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                const Piece* piece = board_.getPiece(Position{row, col});
                if (piece && piece->type() == PieceType::King && piece->color() == PieceColor::White) {
                    king = {row, col};
                }
            }
        }
        sf::RectangleShape danger(sf::Vector2f(cell, cell));
        auto pos = boardToScreen(king);
        danger.setPosition(pos);
        danger.setFillColor(sf::Color(220, 60, 60, 120));
        target.draw(danger);
    }

    if (board_.isInCheck(PieceColor::Black)) {
        Position king = {0, 4};
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                const Piece* piece = board_.getPiece(Position{row, col});
                if (piece && piece->type() == PieceType::King && piece->color() == PieceColor::Black) {
                    king = {row, col};
                }
            }
        }
        sf::RectangleShape danger(sf::Vector2f(cell, cell));
        auto pos = boardToScreen(king);
        danger.setPosition(pos);
        danger.setFillColor(sf::Color(220, 60, 60, 120));
        target.draw(danger);
    }

    if (selectedCell_) {
        sf::RectangleShape selected(sf::Vector2f(cell, cell));
        selected.setPosition(boardToScreen(*selectedCell_));
        selected.setFillColor(sf::Color(75, 156, 211, 110));
        target.draw(selected);

        for (const Move& move : highlightedMoves_) {
            sf::CircleShape hint(cell * 0.12f);
            hint.setOrigin(hint.getRadius(), hint.getRadius());
            auto pos = boardToScreen(move.to);
            hint.setPosition(pos.x + cell / 2.f, pos.y + cell / 2.f);
            hint.setFillColor(move.isCapture ? sf::Color(200, 50, 50, 190) : sf::Color(40, 120, 40, 170));
            target.draw(hint);
        }
    }

    if (board_.lastMoveFrom() && board_.lastMoveTo()) {
        sf::RectangleShape marker(sf::Vector2f(cell, cell));
        marker.setFillColor(sf::Color(255, 224, 130, 80));

        marker.setPosition(boardToScreen(*board_.lastMoveFrom()));
        target.draw(marker);
        marker.setPosition(boardToScreen(*board_.lastMoveTo()));
        target.draw(marker);
    }
}

std::string Game::pieceLabel(const Piece& piece) const {
    const std::string color = piece.color() == PieceColor::White ? "Branca" : "Preta";
    switch (piece.type()) {
        case PieceType::Pawn:
            return "Peão " + color;
        case PieceType::Rook:
            return "Torre " + color;
        case PieceType::Knight:
            return "Cavalo " + color;
        case PieceType::Bishop:
            return "Bispo " + color;
        case PieceType::Queen:
            return "Rainha " + color;
        case PieceType::King:
            return "Rei " + color;
    }
    return "";
}

void Game::drawPieceGlyph(sf::RenderTarget& target, const Piece& piece, sf::Vector2f center, float size) {
    sf::CircleShape token(size * 0.34f);
    token.setOrigin(token.getRadius(), token.getRadius());
    token.setPosition(center);
    token.setFillColor(piece.color() == PieceColor::White ? sf::Color(245, 245, 245) : sf::Color(25, 25, 25));
    token.setOutlineThickness(2.f);
    token.setOutlineColor(piece.color() == PieceColor::White ? sf::Color(50, 50, 50) : sf::Color(220, 220, 220));
    target.draw(token);

    sf::Text text;
    text.setFont(font_);
    text.setCharacterSize(static_cast<unsigned int>(size * 0.25f));
    text.setFillColor(piece.color() == PieceColor::White ? sf::Color(40, 40, 40) : sf::Color(230, 230, 230));

    std::string glyph;
    switch (piece.type()) {
        case PieceType::Pawn:
            glyph = "P";
            break;
        case PieceType::Rook:
            glyph = "R";
            break;
        case PieceType::Knight:
            glyph = "N";
            break;
        case PieceType::Bishop:
            glyph = "B";
            break;
        case PieceType::Queen:
            glyph = "Q";
            break;
        case PieceType::King:
            glyph = "K";
            break;
    }

    text.setString(glyph);
    auto bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    text.setPosition(center);
    target.draw(text);
}

void Game::drawPieces(sf::RenderTarget& target) {
    const float cell = boardRect_.width / 8.f;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = board_.getPiece(Position{row, col});
            if (!piece) {
                continue;
            }

            auto topLeft = boardToScreen(Position{row, col});
            sf::Vector2f center(topLeft.x + cell / 2.f, topLeft.y + cell / 2.f);
            drawPieceGlyph(target, *piece, center, cell);
        }
    }
}

std::string Game::turnLabel() const {
    std::string base = board_.currentTurn() == PieceColor::White ? "Vez: Brancas" : "Vez: Pretas";

    if (board_.isCheckmate(board_.currentTurn())) {
        return base + " | Xeque-mate";
    }
    if (board_.isStalemate(board_.currentTurn())) {
        return base + " | Afogamento";
    }
    if (board_.isInCheck(board_.currentTurn())) {
        return base + " | Xeque";
    }
    return base;
}

void Game::drawStatusPanel(sf::RenderTarget& target) {
    const auto size = window_.getSize();
    const float width = static_cast<float>(size.x);
    const float panelX = boardRect_.left + boardRect_.width + 20.f;
    const float panelWidth = width - panelX - 20.f;

    sf::RectangleShape panel(sf::Vector2f(panelWidth, boardRect_.height));
    panel.setPosition(panelX, boardRect_.top);
    panel.setFillColor(sf::Color(33, 37, 44));
    panel.setOutlineColor(sf::Color(60, 65, 75));
    panel.setOutlineThickness(2.f);
    target.draw(panel);

    sf::Text title("Xadrez", font_, 38);
    title.setPosition(panelX + 22.f, boardRect_.top + 16.f);
    title.setFillColor(sf::Color(245, 245, 245));
    target.draw(title);

    sf::Text turn(turnLabel(), font_, 20);
    turn.setPosition(panelX + 22.f, boardRect_.top + 80.f);
    turn.setFillColor(sf::Color(200, 220, 255));
    target.draw(turn);

    sf::Text message(statusMessage_, font_, 18);
    message.setPosition(panelX + 22.f, boardRect_.top + 124.f);
    message.setFillColor(sf::Color(220, 220, 220));
    target.draw(message);

    sf::RectangleShape restart(sf::Vector2f(restartButtonRect_.width, restartButtonRect_.height));
    restart.setPosition(restartButtonRect_.left, restartButtonRect_.top);
    restart.setFillColor(sf::Color(70, 130, 190));
    restart.setOutlineThickness(1.f);
    restart.setOutlineColor(sf::Color(190, 220, 255));
    target.draw(restart);

    sf::Text restartText("Reiniciar partida", font_, 18);
    restartText.setFillColor(sf::Color::White);
    auto r = restartText.getLocalBounds();
    restartText.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    restartText.setPosition(restartButtonRect_.left + restartButtonRect_.width / 2.f,
                            restartButtonRect_.top + restartButtonRect_.height / 2.f);
    target.draw(restartText);

    if (board_.hasPendingPromotion()) {
        sf::Text promotionTitle("Promoção: escolha a peça", font_, 17);
        promotionTitle.setPosition(panelX + 22.f, restartButtonRect_.top + restartButtonRect_.height + 8.f);
        promotionTitle.setFillColor(sf::Color(255, 220, 120));
        target.draw(promotionTitle);

        for (const auto& [type, rect] : promotionButtons_) {
            sf::RectangleShape button(sf::Vector2f(rect.width, rect.height));
            button.setPosition(rect.left, rect.top);
            button.setFillColor(sf::Color(80, 88, 105));
            button.setOutlineColor(sf::Color(190, 190, 190));
            button.setOutlineThickness(1.f);
            target.draw(button);

            std::string label;
            switch (type) {
                case PieceType::Queen:
                    label = "Rainha";
                    break;
                case PieceType::Rook:
                    label = "Torre";
                    break;
                case PieceType::Bishop:
                    label = "Bispo";
                    break;
                case PieceType::Knight:
                    label = "Cavalo";
                    break;
                default:
                    break;
            }

            sf::Text text(label, font_, 17);
            auto bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            text.setPosition(rect.left + rect.width / 2.f, rect.top + rect.height / 2.f);
            text.setFillColor(sf::Color(240, 240, 240));
            target.draw(text);
        }
    }
}

}  // namespace chess
