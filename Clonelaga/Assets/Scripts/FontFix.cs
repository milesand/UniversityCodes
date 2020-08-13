using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FontFix : MonoBehaviour {

    public Font[] fonts;

	void Start() {
		foreach (Font f in fonts) {
            f.material.mainTexture.filterMode = FilterMode.Point;
        }
	}
}
