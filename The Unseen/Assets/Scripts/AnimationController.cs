using System.Collections;

using System.Collections.Generic;

using UnityEngine;



public class AnimationController : MonoBehaviour {

    private float change = 0.0f;

    private bool isDark = true;

	// Use this for initialization

	void Start () {
        transform.GetChild(1).gameObject.GetComponent<SkinnedMeshRenderer>().material.color = Color.black;

    }

    // Update is called once per frame
    void Update () {

        change += Time.deltaTime;

        if (change > 1.0f){

            if (isDark){
                transform.GetChild(1).gameObject.GetComponent<SkinnedMeshRenderer>().material.color = Color.white;

            }

            else{
                transform.GetChild(1).gameObject.GetComponent<SkinnedMeshRenderer>().material.color = Color.black;

            }

            isDark = !isDark;

            change = 0.0f;

        }

	}

}

