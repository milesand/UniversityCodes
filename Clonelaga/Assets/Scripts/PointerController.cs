using System.Collections.Generic;

using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class PointerController : MonoBehaviour {

	private GameObject pointer;
	private EventSystem eventSystem;
	private List<GameObject> buttons;
    private AudioSource audioSource;

	private int selectedButton;
	private float timeInterval;

	private void Start() {
		pointer = GameObject.Find("Pointer");
		eventSystem = GameObject.Find("EventSystem").GetComponent<EventSystem>();
		buttons = new List<GameObject>();
		Transform buttons_t = GameObject.Find("Buttons").transform;
		foreach (Transform t in buttons_t) {
			buttons.Add(t.gameObject);
		}

        audioSource = GetComponent<AudioSource>();

		selectedButton = -1;
	}

	public void SelectButton(int n) {
		MovePointer(buttons[n]);
		selectedButton = n;
		eventSystem.SetSelectedGameObject(buttons[n]);
        audioSource.Play();
	}
	
	private void MovePointer(GameObject button) {
		if (!pointer.GetComponent<Text>().enabled) {
			pointer.GetComponent<Text>().enabled = true;
		}
		Vector3 temp = pointer.transform.position;
		temp.y = button.transform.position.y;
		pointer.transform.position = temp;
	}

	private void Update() {
		if (timeInterval > 0) {
			timeInterval -= Time.deltaTime;
		}
		if (timeInterval <= 0) {
            int input = (int)Input.GetAxisRaw("Vertical");
            if (input != 0) {
				if (selectedButton < 0) {
					SelectButton(0);
				} else {
					int len = buttons.Count;
					SelectButton(n: ((selectedButton - input) % len + len) % len);
				}
				timeInterval = 0.1f;
			}
		}
	}

	private void OnDisable() {
		selectedButton = -1;
		eventSystem.SetSelectedGameObject(null);
		pointer.GetComponent<Text>().enabled = false;
	}
}
