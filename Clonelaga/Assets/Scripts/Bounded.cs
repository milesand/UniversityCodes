using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class Bounded : MonoBehaviour {
	void Update () {
        if (!(Math.Abs(transform.position.x) < 9.6f && Math.Abs(transform.position.y) < 5.4f)) {
            Destroy(gameObject);
        }
    }
}
