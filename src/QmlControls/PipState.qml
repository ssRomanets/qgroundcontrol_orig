import QtQuick

Item {
    id: control
    state:  initState

    readonly property string initState: "init"
    readonly property string pipState:  "pip"
    readonly property string fullState: "full"
    readonly property string windowState: "window"

    property var pipView

    property var m_viewControl: control.parent

    states: [
        State{
            name: pipState

            AnchorChanges {
                target: m_viewControl
                anchors.top: pipView.m_pipContentItem.top
                anchors.bottom: pipView.m_pipContentItem.bottom
                anchors.left: pipView.m_pipContentItem.left
                anchors.right: pipView.m_pipContentItem.right
            }

            ParentChange {
                target: m_viewControl
                parent: pipView.m_pipContentItem
            }
        },

        State {
            name: fullState

            AnchorChanges {
                target:         m_viewControl
                anchors.top:    pipView.parent.top
                anchors.bottom: pipView.parent.bottom
                anchors.left:   pipView.parent.left
                anchors.right:  pipView.parent.right
            }

            ParentChange {
                target: m_viewControl
                parent: pipView.parent
            }
        },
        State {
            name: windowState

            AnchorChanges {
                target:         m_viewControl
                anchors.top:    pipView.m_windowContentItem.top
                anchors.bottom: pipView.m_windowContentItem.bottom
                anchors.left:   pipView.m_windowContentItem.left
                anchors.right:  pipView.m_windowContentItem.right
            }

            ParentChange {
                target: m_viewControl
                parent: pipView.m_windowContentItem
            }

            StateChangeScript {
                script: {
                    pipView.showWindow()
                }
            }
        }
    ]
}
