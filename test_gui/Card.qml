import QtQuick 2.15

Rectangle {
    id: card
    width: 300
    height: 150
    radius: 10
    color: parent.isDarkMode ? "#2C2F33" : "#FFFFFF"
    border.color: parent.isDarkMode ? "#99AAB5" : "#CCCCCC"
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10

        Text {
            id: cardTitle
            text: "Card Title"
            font.pixelSize: 18
            color: parent.isDarkMode ? "#FFFFFF" : "#000000"
        }

        Text {
            id: cardValue
            text: "$0.00"
            font.pixelSize: 24
            font.bold: true
            color: parent.isDarkMode ? "#FFFFFF" : "#000000"
        }
    }
}
