using UnityEngine;
using System;

public class Sway : MonoBehaviour {

    public float range;
    public float speed;

    private float direction = 1.0f;
	
	// Update is called once per frame
	void Update () {
        this.transform.Translate(direction * speed * Time.deltaTime, 0.0f, 0.0f);
        if (Math.Abs(this.transform.position.x) > this.range) {
            this.direction = -this.direction;
        }
	}
}
