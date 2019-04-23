using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Switch_Tutorial : MonoBehaviour {

    public GameObject player;
    public GameObject PopupText;
    public Text theText;

    void Start () {

        theText.text = "The big lever ahead is a Light Switch that turn some lights on/off.";

	}
	
	void Update () {
        if (player.transform.position.z >= -11f)
        {
            PopupText.SetActive(false);
        }

            if (player.transform.position.z >= -9.5f && player.transform.position.z <= -6.0f)
        {
            theText.text = "Now go ahead";
        }

        if (player.transform.position.z >= -6.0f)
        {
            theText.text = "The button ahead is a Wall Switch that make some wall disappear/appear.";
        }
	}
}
