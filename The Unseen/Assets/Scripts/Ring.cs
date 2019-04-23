using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ring : MonoBehaviour {

	// Use this for initialization
	private GameObject player;
	void Start () {
        player = GameObject.FindGameObjectWithTag("Player");
	}
	
	// Update is called once per frame
	void Update () {
		transform.position = player.transform.position;
		transform.rotation = player.transform.rotation;
        transform.Rotate(Vector3.up * 98, Space.World);
	}
}
