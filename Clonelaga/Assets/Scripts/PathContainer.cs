using UnityEngine;

public class PathContainer : MonoBehaviour {

    public Transform[] Waypoints;

    void OnDrawGizmos() {
        if (Waypoints == null || Waypoints.Length == 0 || Waypoints[0] == null) {
            return;
        }
        Gizmos.color = Color.green;
        Gizmos.DrawIcon(Waypoints[0].position, "Light Gizmo.tiff", true);
        for (int i = 1; i < Waypoints.Length; i += 1) {
            if (Waypoints[i] == null) {
                return;
            }
            Gizmos.DrawLine(Waypoints[i - 1].position, Waypoints[i].position);
            Gizmos.DrawIcon(Waypoints[i].position, "Light Gizmo.tiff", true);
        }
    }
}
