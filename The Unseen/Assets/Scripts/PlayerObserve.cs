using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerObserve : MonoBehaviour {

    public KeyCode observeKey;
    public KeyCode observeKeyController;
    public Camera MainCamera;
    public Camera ObserveCamera;
	
	// Update is called once per frame
	void Update () {
        if (ObserveCamera.gameObject.activeSelf ==  false)
        {
            if (Input.GetKeyUp(observeKey) || Input.GetKeyUp(observeKeyController))
            {
                //MainCamera.gameObject.SetActive(false);
                ObserveCamera.gameObject.SetActive(true);
            }
        }
        else
        {
            if (Input.GetKeyUp(observeKey) || Input.GetKeyUp(observeKeyController))
            {
               // MainCamera.gameObject.SetActive(true);
                ObserveCamera.gameObject.SetActive(false);
            }
        }
	}
}
