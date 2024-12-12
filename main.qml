import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1280
    height: 720
    title: "Crypto Dashboard"

    // 라이트/다크 테마 설정 (property 사용)
    property alias isDarkMode: darkModeCheckbox.checked
    property color backgroundColor: isDarkMode ? "#202225" : "#F5F5F5"
    property color textColor: isDarkMode ? "#FFFFFF" : "#000000"

    // 배경 색상 적용
    Rectangle {
        anchors.fill: parent
        color: backgroundColor

        // 기본 레이아웃
        Column {
            spacing: 20
            anchors.centerIn: parent
            width: parent.width * 0.9

            // Title
            Text {
                id: dashboardTitle
                text: "Crypto Dashboard"
                font.pixelSize: 32
                color: textColor
                anchors.horizontalCenter: parent.horizontalCenter
            }

            // 다크모드 스위치
            Row {
                spacing: 10
                CheckBox {
                    id: darkModeCheckbox
                }
                Text {
                    text: "Dark Mode"
                    color: textColor
                }
            }
        }

        // 대시보드 레이아웃
        Column {
            spacing: 20
            width: parent.width * 0.9
            anchors.centerIn: parent

            // 카드 그리드 레이아웃
            GridLayout {
                columns: 3

                // 카드 1: Total Balance
                Rectangle {
                    width: parent.width / 3 - 10
                    height: 150
                    color: "#FFFFFF"
                    border.color: "#CCCCCC"
                    border.width: 1
                    radius: 10

                    Column {
                        spacing: 10
                        anchors.centerIn: parent
                        Text {
                            text: "Total Balance"
                            font.pixelSize: 18
                            color: textColor
                        }
                        Text {
                            text: "$12,345.67"
                            font.pixelSize: 24
                            font.bold: true
                            color: textColor
                        }
                    }
                }

                // 카드 2: Income
                Rectangle {
                    width: parent.width / 3 - 10
                    height: 150
                    color: "#FFFFFF"
                    border.color: "#CCCCCC"
                    border.width: 1
                    radius: 10

                    Column {
                        spacing: 10
                        anchors.centerIn: parent
                        Text {
                            text: "Income"
                            font.pixelSize: 18
                            color: textColor
                        }
                        Text {
                            text: "$8,123.45"
                            font.pixelSize: 24
                            font.bold: true
                            color: textColor
                        }
                    }
                }

                // 카드 3: Expenses
                Rectangle {
                    width: parent.width / 3 - 10
                    height: 150
                    color: "#FFFFFF"
                    border.color: "#CCCCCC"
                    border.width: 1
                    radius: 10

                    Column {
                        spacing: 10
                        anchors.centerIn: parent
                        Text {
                            text: "Expenses"
                            font.pixelSize: 18
                            color: textColor
                        }
                        Text {
                            text: "$4,567.89"
                            font.pixelSize: 24
                            font.bold: true
                            color: textColor
                        }
                    }
                }
            }
        }
    }
}
