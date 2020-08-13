using UnityEngine;
using UnityEngine.Events;
using System;

public enum EnemyState {
    Idle,
    Attacking
}

public class EnemyController : MonoBehaviour {

    public float attackCooldownMin;
    public float attackCooldownMax;
    public float fireRate;

    public GameObject shot;

    public AudioClip deathSound;
    public GameObject deathEffect;
    public AudioClip attackSound;
    public Transform shotLocation;

    public int scoreValueIdle;
    public int scoreValueMoving;
    public int id;
    private float timeUntilAttack;
    public EnemyState state;

    public GameObject[] attackPaths;
    private Transform enemyField;

    private GameController gameController;
    public Transform fighterRespawn;
    private FighterController fighter;
    private CircleCollider2D cc;
    private AudioSource sfx;
    public EnemyMover movement;

    public static Vector3 IdToPosition(int id) {
        if (id < 10) {
            return new Vector3(-2.7f + 0.6f * id, 1.8f, 0.0f);
        } else if (id < 20) {
            return new Vector3(-2.7f + 0.6f * (id - 10), 2.4f, 0.0f);
        } else if (id < 28) {
            return new Vector3(-2.1f + 0.6f * (id - 20), 3.0f, 0.0f);
        } else {
            return new Vector3(-2.1f + 0.6f * (id - 28), 3.6f, 0.0f);
        }
    }

    public Vector3 PositionInLine() {
        return IdToPosition(id);
    }

    public void Spawn(int level, int id, PathContainer path) {
        this.id = id;
        this.state = EnemyState.Attacking;
        if (this.movement == null) {
            this.movement = this.GetComponent<EnemyMover>();
        }
        this.movement.speed = 6.0f + Mathf.Log((float)level, 4.0f);
        if (id < 20) { // Zako
            this.fireRate = 0.2f - 6.4f / ((float)level + 31.0f);
            this.attackCooldownMin = 5.0f + 30.0f / (5.0f + (float)level);
            this.attackCooldownMax = 2.0f * this.attackCooldownMin;
        } else {
            this.fireRate = 0.25f - 6.4f / ((float)level + 30.0f);
            this.attackCooldownMin = 4.0f + 30.0f / (5.0f + (float)level);
            this.attackCooldownMax = 1.8f * this.attackCooldownMin;
        }
        var pathFinished = new UnityEvent();

        // Welcome to callback hell
        pathFinished.AddListener(
            () => {
                var inPosition = new UnityEvent();
                inPosition.AddListener(
                    () => {
                        this.transform.rotation = Quaternion.LookRotation(Vector3.forward);
                        this.state = EnemyState.Idle;
                    }
                );
                this.movement.MoveToPos(enemyField, this.PositionInLine(), inPosition);
            }
        );
        this.GetComponent<EnemyMover>().FollowPath(path, pathFinished);
    }

    void Start() {
        this.cc = this.GetComponent<CircleCollider2D>();
        this.cc.enabled = true;
        if (this.movement == null) {
            this.movement = this.GetComponent<EnemyMover>();
        }
        this.enemyField = GameObject.FindWithTag("Enemies").GetComponent<Transform>();
        this.gameController = GameObject.FindWithTag("GameController").GetComponent<GameController>();
        this.fighterRespawn = GameObject.FindWithTag("Respawn").transform;
        this.fighter = this.fighterRespawn.GetComponentInChildren<FighterController>();
        this.sfx = GameObject.FindWithTag("Audio_SFX").GetComponent<AudioSource>();
        this.timeUntilAttack = this.attackCooldownMin + UnityEngine.Random.value * (this.attackCooldownMax-this.attackCooldownMin);
        this.state = EnemyState.Idle;
	}
    
    void OnTriggerEnter2D(Collider2D other) {
        if (other.gameObject.tag == "FighterShot") {
            Destroy(other.gameObject);
            if (this.movement.IsMoving()) {
                this.Die(scoreValueMoving);
            } else {
                this.Die(scoreValueIdle);
            }
        } else if (other.gameObject.tag == "Player") {
            this.Die(0);
        }
    }

    void Update() {
        if (this.state == EnemyState.Idle) {
            if (this.fighter.alive) {
                this.timeUntilAttack -= Time.deltaTime;
                if (this.timeUntilAttack < 0.0f) {
                    this.timeUntilAttack = this.attackCooldownMin + UnityEngine.Random.value * (this.attackCooldownMax - this.attackCooldownMin);
                    this.Attack();
                }
            }
        } else if (this.state == EnemyState.Attacking) {
            if (UnityEngine.Random.value <= this.fireRate) {
                Debug.Log(Vector3.Angle(this.transform.up, this.transform.position - fighter.transform.position));
                if (Math.Abs(Vector3.Angle(this.transform.up, fighter.transform.position - this.transform.position)) < 20) {
                    Instantiate(this.shot, this.shotLocation.position, this.shotLocation.rotation);
                }
            }
        }
        
    }

    void Attack() {
        this.state = EnemyState.Attacking;
        this.sfx.PlayOneShot(this.attackSound);
        this.transform.parent = null;
        int idx = UnityEngine.Random.Range(0, this.attackPaths.Length - 1);
        PathContainer path = Instantiate(this.attackPaths[idx], this.transform.position, Quaternion.identity).GetComponent<PathContainer>();
        var pathComplete = new ECEvent();
        pathComplete.AddListener(() => {
            Destroy(path.gameObject);
            var backAgain = new ECEvent();
            backAgain.AddListener(() => {
                this.state = EnemyState.Idle;
                this.transform.parent = enemyField;
                this.transform.rotation = Quaternion.LookRotation(Vector3.forward);
            });
            this.movement.MoveToPos(enemyField, this.PositionInLine(), backAgain);
        });
        this.movement.FollowPath(path, pathComplete);
    }

    void Die(int score) {
        this.cc.enabled = false;
        if (this.movement.path != null && this.movement.path.gameObject.tag == "AttackPath") {
            Destroy(this.movement.path.gameObject);
        }
        gameController.EnemyDiedWithScore(score);
        this.sfx.PlayOneShot(this.deathSound);
        Instantiate(deathEffect, transform.position, Quaternion.identity);
        Destroy(this.gameObject);
    }
}

public class ECEvent : UnityEvent { }
