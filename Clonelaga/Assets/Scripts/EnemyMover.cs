using UnityEngine;
using UnityEngine.Events;

public enum MovementState {
    Stopped,
    FollowingPath,
    MovingToPos
}

public class EnemyMover : MonoBehaviour {
    private MovementState state;

    public float speed;
    public float proximity;

    private UnityEvent callback = null;

    // For FollowingPath
    private int WaypointId = 0;
    public PathContainer path = null;

    // For MovingToPos
    private Transform parent;
    private Vector3 destination;

    public void FollowPath(PathContainer newPath) {
        FollowPath(newPath, null);
    }

    public void FollowPath(PathContainer newPath, UnityEvent callback) {
        state = MovementState.FollowingPath;
        path = newPath;
        transform.position = path.transform.position;
        WaypointId = 0;
        this.callback = callback;
    }

    public void MoveToPos(Transform parent, Vector3 destination) {
        MoveToPos(parent, destination, null);
    }

    public void MoveToPos(Transform parent, Vector3 destination, UnityEvent callback) {
        state = MovementState.MovingToPos;
        this.parent = parent;
        this.destination = destination;
        this.callback = callback;
    }

    public bool IsMoving() {
        return state != MovementState.Stopped;
    }

    public void Stop() {
        state = MovementState.Stopped;
        if (callback != null) {
            var temp_callback = callback;
            callback = null;
            temp_callback.Invoke();
        }
    }

    void Update() {
        if (state == MovementState.FollowingPath) {

            if (path.Waypoints.Length <= WaypointId) {
                Stop();
                return;
            }
            if ((path.Waypoints[WaypointId].position - transform.position).magnitude <= proximity) {
                WaypointId += 1;
                if (path.Waypoints.Length <= WaypointId) {
                    Stop();
                    return;
                }
            }
            Vector3 dir = (path.Waypoints[WaypointId].position - transform.position).normalized;
            float angle = Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg - 90;
            transform.rotation = Quaternion.AngleAxis(angle, Vector3.forward);
            transform.position += transform.up * Time.deltaTime * speed;

        } else if (state == MovementState.MovingToPos) {
            Vector3 dir = (parent.position + destination - transform.position);
            if (dir.magnitude <= proximity) {
                transform.parent = parent;
                transform.localPosition = destination;
                Stop();
                return;
            }
            float angle = Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg - 90;
            transform.rotation = Quaternion.AngleAxis(angle, Vector3.forward);
            transform.position += transform.up * Time.deltaTime * speed;
        }
    }
}
