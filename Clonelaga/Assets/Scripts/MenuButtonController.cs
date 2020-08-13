using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class MenuButtonController : MonoBehaviour, IPointerEnterHandler {

	public int n;
	public GameObject panel;
	private PointerController pointerController;

	void Start () {
		pointerController = panel.GetComponent<PointerController>();
	}

	public void OnPointerEnter(PointerEventData eventData) {
		pointerController.SelectButton(n);
	}
}
