using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScrollBackground : MonoBehaviour {

    public float speed;

    private Transform below;
    private Transform above;

	private void Start() {
        below = this.gameObject.transform.GetChild(0);
        above = this.gameObject.transform.GetChild(1);
        below.SetPositionAndRotation(new Vector3(0.0f, 0.0f, 10.0f), new Quaternion());
        above.SetPositionAndRotation(new Vector3(0.0f, 10.80f, 10.0f), new Quaternion());
	}
	
	private void Update() {
        float dt = Time.deltaTime;
        below.Translate(new Vector3(0.0f, - dt * speed, 0.0f));
        above.Translate(new Vector3(0.0f, - dt * speed, 0.0f));
        if (below.position.y <= -10.8f) {
            below.SetPositionAndRotation(new Vector3(0.0f, above.position.y + 10.8f, 10.0f), new Quaternion());
            Transform temp = below;
            below = above;
            above = temp;
        }
	}
}
