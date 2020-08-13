using UnityEngine;

public class Mover : MonoBehaviour {
    // move in straight line
    public float x_speed;
    public float y_speed;
    
	void Update () {
        transform.Translate(x_speed * Time.deltaTime, y_speed * Time.deltaTime, 0.0f);
	}
}
